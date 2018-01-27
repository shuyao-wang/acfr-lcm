#include <iostream>
#include <list>
#include <vector>
#include <tuple>

#include <chrono>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <cstdio> // for perror
#include <csignal>

#include <lcm/lcm-cpp.hpp>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>

// for crc32
#include <zlib.h>

#include <bot_param/param_client.h>
#include "acfr-common/serial.h"
#include "acfr-common/timestamp.h"

#include "perls-lcmtypes++/senlcm/evologics_modem_t.hpp"
#include "perls-lcmtypes++/senlcm/evologics_usbl_t.hpp"
#include "perls-lcmtypes++/senlcm/evologics_usbl_angles_t.hpp"
#include "perls-lcmtypes++/senlcm/evologics_command_t.hpp"
#include "perls-lcmtypes++/senlcm/evologics_range_t.hpp"
#include "perls-lcmtypes++/perllcm/heartbeat_t.hpp"

#define DTOR M_PI/180.0
#define RTOD 180.0/M_PI

struct ChannelSubscription
{
    ChannelSubscription(std::string regex, bool use_pbm, lcm::Subscription *subscription)
        : channel_regex(regex), use_pbm(use_pbm), subscription(subscription)
    {
    
    }

    std::string channel_regex;
    bool use_pbm;
    lcm::Subscription *subscription;
};

struct PingTarget
{
    PingTarget(std::string target_name, uint8_t target_id, int period)
        : target_name(target_name), target_id(target_id),
        tracking_active(true), send_fixes(false), minimum_period(period),
        last_sent_fix_time(0)
    {
    
    }

    std::string target_name;
    uint8_t target_id;
    bool tracking_active;
    bool send_fixes;
    int64_t minimum_period;
    int64_t last_sent_fix_time;
    int64_t last_ping_time;
};

enum 
{
    MSG_PBM=0,
    MSG_BURST,
    MSG_IM,
    MSG_IMS,
    MSG_TYPES
};

class EvologicsModem
{
public:
    EvologicsModem();
    ~EvologicsModem();

    // to initialise/setup the modem connection
    bool load_configuration(char *program_name);
    bool connect_modem();
    bool configure_modem();

    // the three handlers we need for incoming LCM messages
    void on_lcm_data(const lcm::ReceiveBuffer* rbuf, const std::string &channel);
    void on_lcm_pbm_data(const lcm::ReceiveBuffer* rbuf, const std::string &channel);
    // the usbl fix handling has special cases the other messages do not
    // so we make it separate to simplify handling
    void on_usbl_fix(const lcm::ReceiveBuffer* rbuf, const std::string &channel);
    void on_heartbeat(const lcm::ReceiveBuffer* rbuf, const std::string &channel, const perllcm::heartbeat_t *hb);
    void on_evo_control(const lcm::ReceiveBuffer* rbuf, const std::string &channel, const senlcm::evologics_command_t *ec);

    std::vector<unsigned char> build_lcm_data_message(unsigned char *d, int size, int target, const char *dest_channel, bool use_pbm);
    bool send_message(int message_type, char const *data, int length);

    void modem_read_thread();
    void lcm_handle_thread();
    void parse_recv(std::vector<uint8_t> &buf);
    void parse_generic(std::vector<uint8_t> &buf);

    void modem_write(char const *command, int length);
    bool send_query(const char *d);
    bool send_command(const char *d);
    bool send_mode(const char *d);

    int get_target_channel(std::string const &channel);

    void run();

    void publish_modem_response(int64_t timestamp, const std::vector<uint8_t> &buffer);
    void queue_modem_response(int64_t timestamp, std::vector<uint8_t> &buffer);

    void process_burst(int64_t timestamp, std::vector<uint8_t> &v);
    void process_im(int64_t timestamp, std::vector<uint8_t> &v);
    void process_pbm(int64_t timestamp, std::vector<uint8_t> &v);

    void process_usbllong(int64_t timestamp, std::vector<uint8_t> &v);
    void process_usblangles(int64_t timestamp, std::vector<uint8_t> &v);

    void process_lcm_data(uint8_t *data, int data_length);

private:
    lcm::LCM lcm;

    // handle to modem socket/serial
    int modem_fd;

    std::mutex write_mutex;

    bool exit_loops;

    // serial parameters
    bool use_serial_comm;
    char *device;
    int baud;
    char *parity;

    // eth parameters
    bool use_ip_comm;
    char *ip;
    char *port;

    // command/reponse line ending
    char *term;
    int term_len;

    // local vehicle name
    std::string vehicle_name;

    // timeout waiting for acknowledgement
    // this influences the minimum ping period
    int16_t ack_timeout;

    // signal properties
    int gain;
    int source_level;
    bool auto_gain;
    int sound_speed;

    // handling the LCM tunnel to all platforms
    // these are per-regex and will detect the target
    // on the fly (all are handled by the same function)
    std::list<ChannelSubscription> lcm_channels;

    // handling who is being pinged, and how often
    // this also handles the USBL_FIX channels
    // not leaving that to the other channel subscription
    std::list<PingTarget> ping_targets;
    int topside_id;

    std::condition_variable response_added;
    std::mutex queue_response_mutex;
    std::list<std::pair<int64_t, std::vector<uint8_t>>> queued_responses;

    std::condition_variable message_added;
    std::mutex queue_message_mutex;
    std::list<std::pair<int, std::vector<uint8_t>>> queued_messages;
    std::vector<uint8_t> next_ping;
};

bool starts_with(std::string const &full_string, std::string const &prefix)
{
    return 0 == full_string.compare(0, prefix.size(), prefix);
}

bool starts_with(std::string const &full_string, char const *prefix)
{
    return 0 == full_string.compare(0, strlen(prefix), prefix);
}

// The GLOBALS to bind to the signal handler
int loop_exit;
int pipe_broken;

void signal_handler(int sig)
{
    std::cerr << "WARNING: Caught signal: " << sig << std::endl;
    if(sig == SIGPIPE)
        // the connection to the Evologics has failed for some reason,
        // we will just reconnect it
        pipe_broken = 1;
    else
        loop_exit = 1;
}

EvologicsModem::EvologicsModem()
    : lcm(lcm::LCM()), topside_id(0)
{
    lcm.subscribe("HEARTBEAT_1HZ", &EvologicsModem::on_heartbeat, this);
    lcm.subscribe("EVOLOGICS_CONTROL", &EvologicsModem::on_evo_control, this);
};

EvologicsModem::~EvologicsModem()
{
}

bool EvologicsModem::load_configuration(char *program_name)
{
    BotParam *param = NULL;
    param = bot_param_new_from_server (this->lcm.getUnderlyingLCM(), 1);
    if(param == NULL)
        return false;
        
    char rootkey[64];        
    char key[128];
    sprintf (rootkey, "sensors.%s", program_name);

    use_serial_comm = false;
    use_ip_comm = false;

    // check if we are using an serial connection
    sprintf(key, "%s.device", rootkey);
    if (bot_param_has_key(param, key))
    {
        device = bot_param_get_str_or_fail(param, key);

        sprintf(key, "%s.baud", rootkey);
        baud = bot_param_get_int_or_fail(param, key);

        sprintf(key, "%s.parity", rootkey);
        parity = bot_param_get_str_or_fail(param, key);
        use_serial_comm = true;
    }

    // check if we are using an IP connection
    sprintf(key, "%s.ip", rootkey);
    if (bot_param_has_key(param, key))
    {
        ip = bot_param_get_str_or_fail(param, key);
        sprintf(key, "%s.port", rootkey);
        port = bot_param_get_str_or_fail(param, key);
        use_ip_comm = true;
    }
    if (use_serial_comm == false && use_ip_comm == false) {
        std::cerr << "Missing config setting for ip or serial comms" << std::endl;
        exit(1);
    }

    sprintf(key, "%s.term", rootkey);
    if (bot_param_has_key(param, key))
    {
        int terms[4];
        int num_term = bot_param_get_int_array(param, key, terms, 4);
        term = (char *)malloc(num_term);
        for(int i=0; i<num_term; i++)
            term[i] = terms[i] & 0xFF;
        term_len = num_term;
    }
    else
    {
        term = (char *)malloc(1);
        if (use_serial_comm)
        {
            *term = '\r';
            term_len = 1;
        }
            
        if (use_ip_comm)
        {
            *term = '\n';
            term_len = 1;
        }
    }

    sprintf(key, "%s.vehicle_name", rootkey);
    vehicle_name = bot_param_get_str_or_fail(param, key);

    lcm.subscribe(vehicle_name + "\\.USBL_FIX\\..*", &EvologicsModem::on_usbl_fix, this);
    
    sprintf(key, "%s.ack_timeout", rootkey);
    ack_timeout = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.gain", rootkey);
    gain = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.source_level", rootkey);
    source_level = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.auto_gain", rootkey);
    auto_gain = bot_param_get_boolean_or_fail(param, key);

    sprintf(key, "%s.sound_speed", rootkey);
    sound_speed = bot_param_get_int_or_fail(param, key);

    sprintf(key, "%s.topside_id", rootkey);
    topside_id = bot_param_get_int_or_fail(param, key);


    // we want to read in default configuration of the channels
    // and targets that are active
    sprintf(key, "%s.lcm", rootkey);
    char **lcm_channels = nullptr;
    lcm_channels = bot_param_get_str_array_alloc(param, key);

    if (lcm_channels)
    {
        int ii = 0;
        while (lcm_channels[ii] != nullptr)
        {
            lcm::Subscription *sub = lcm.subscribe(lcm_channels[ii], &EvologicsModem::on_lcm_data, this);
            this->lcm_channels.push_back(ChannelSubscription(lcm_channels[ii], false, sub));
            std::cout << "Subscribed: " << lcm_channels[ii] << std::endl;
            ++ii;
        }

        bot_param_str_array_free(lcm_channels);
    }


    sprintf(key, "%s.lcm_pbm", rootkey);
    lcm_channels = bot_param_get_str_array_alloc(param, key);

    if (lcm_channels)
    {
        int ii = 0;
        while (lcm_channels[ii] != nullptr)
        {
            lcm::Subscription *sub = lcm.subscribe(lcm_channels[ii], &EvologicsModem::on_lcm_pbm_data, this);
            this->lcm_channels.push_back(ChannelSubscription(lcm_channels[ii], true, sub));
            std::cout << "Subscribed: " << lcm_channels[ii] << std::endl;
            ++ii;
        }

        bot_param_str_array_free(lcm_channels);
    }

    sprintf(key, "%s.targets", rootkey);
    int targets[255];
    int num_targets = bot_param_get_int_array(param, key, targets, 255);

    sprintf(key, "%s.ping_rates", rootkey);
    int ping_rates[255];
    int num_rates = bot_param_get_int_array(param, key, targets, 255);

    sprintf(key, "%s.send_fixes", rootkey);
    int send_fixes[255];
    int num_send_fixes = bot_param_get_boolean_array(param, key, targets, 255);

    sprintf(key, "%s.target_names", rootkey);
    char **target_names = nullptr;
    target_names = bot_param_get_str_array_alloc(param, key);

    if (target_names)
    {
        int ii = 0;
        while (target_names[ii] != nullptr && ii < num_targets)
        {
            // set the period as just larger than the timeout
            this->ping_targets.push_back(PingTarget(target_names[ii], targets[ii], ack_timeout+2));
            std::cout << "Ping target: " << target_names[ii] << std::endl;
            ++ii;
        }

        if (ii != num_targets)
        {
            std::cerr << "Target IDs and name arrays do not match" << std::endl;
            exit(1);
        }

        bot_param_str_array_free(target_names);
    }
    else
    {
        if (num_targets > 0)
        {
            std::cerr << "Target IDs specified with no names" << std::endl;
            exit(1);
        }
    }


    return 0;
}

bool EvologicsModem::connect_modem()
{
    bool success = false;

    if (this->use_serial_comm)
    {
        this->modem_fd = serial_open(
                    this->device,
                    serial_translate_speed(this->baud),
                    serial_translate_parity(parity),
                    1
                );

        serial_set_noncanonical(this->modem_fd, 1, 0);

        tcflush(this->modem_fd, TCIOFLUSH);
        success = true;
    }
    else if (this->use_ip_comm)
    {
        struct addrinfo hints, *evo_addr, *result;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = 0;
        hints.ai_protocol = IPPROTO_TCP;

        int s = getaddrinfo(ip, port, &hints, &result);

        if (s != 0)
        {
            std::cerr << "getaddrinfo: " << gai_strerror(s) << std::endl;
            exit(1);
        }

        for (evo_addr = result; evo_addr != NULL; evo_addr = evo_addr->ai_next)
        {
            this->modem_fd = socket(evo_addr->ai_family, evo_addr->ai_socktype, evo_addr->ai_protocol);
            if (this->modem_fd == -1)
            {
                perror("Could not create socket\n");
                success = false;
                continue;
            }

            if(connect(this->modem_fd, evo_addr->ai_addr, evo_addr->ai_addrlen) != -1)
            {
                success = true;
                break; // Success
            }

            perror("Failed to connect.  Trying next address");
            success = false;
            close(this->modem_fd);
            this->modem_fd = -1;
        }

        freeaddrinfo(result);

        if (evo_addr == NULL)
        {
            std::cerr << "Could not connect to " << ip << " on port " << port << std::endl;
            success = false;
        }
        else
        {
            std::cerr << "Successfully connected to " << ip << " on port " << port << std::endl;
            success = true;
        }

    }
    else
    {
        std::cerr << "Unknown communications protocol.\nCannot connect to modem." << std::endl;
        exit(1);
    }

    
    return success;
}

void EvologicsModem::modem_write(char const *command, int length)
{
    // (this includes many RECV and SEND cmds, but not all!)
    //std::cout << "Sending to modem:\n<<" << 
    std::cout << "<<" << 
        std::string(command, strcspn(command, ",\n")) << std::endl;
    /*senlcm::evologics_modem_t msg;
    msg.utime = timestamp;
    msg.size = buf.size() - 1;
    msg.data = buf;

    char channel_name[128];
    snprintf(channel_name, 128, "%s.EVOLOGICS_LOG", vehicle_name.c_str());
    this->lcm.publish(channel_name, &msg);
    */
    std::lock_guard<std::mutex> lg(this->write_mutex);

    int bytes = 0;


    while (bytes != length)
    {
        bytes = write(this->modem_fd, command, length);

        if (bytes == -1)
        {
            perror("Failed to write to modem.");
            close(this->modem_fd);
            this->connect_modem();
        }
        else if (bytes != length)
        {
            // failed to write
            perror("Failed to write to modem.");
        }
    }
}

bool EvologicsModem::send_query(const char *d)
{
    size_t data_length = strlen(d);

    char *command = (char *)malloc(data_length + term_len);
    memcpy(command, d, data_length);
    memcpy(command + data_length, term, term_len);

    if (d[2] != '?')
    {
        std::cerr << "Attempting to send non-query via send_query:\n>"
            << d << "\n";
        return false;
    }

    this->modem_write(command, data_length + term_len);

    // most queries have a single response line
    // but two of them (that I've found) have more
    int expected_lines = 1;

    if (strlen(d) == 4)
    {
        switch (d[3])
        {
            case 'S':
                // general state
                expected_lines = 5;
                break;
            case 'P':
                // multipathing
                expected_lines = 8;
                break;
        }

    }

    // now we wait for the response
    std::unique_lock<std::mutex> ul(this->queue_response_mutex);
    while (expected_lines > 0)
    {
        while (this->queued_responses.size() == 0)
        {
            this->response_added.wait(ul);
        }

        auto message = this->queued_responses.front();
        this->queued_responses.pop_front();

        std::string message_text((char *)message.second.data(), message.second.size());

        expected_lines--;
        std::cout << "=>" << message_text;
    }

    return true;
}

bool EvologicsModem::send_command(const char *d)
{
    size_t data_length = strlen(d);

    char *command = (char *)malloc(data_length + term_len);
    memcpy(command, d, data_length);
    memcpy(command + data_length, term, term_len);

    this->modem_write(command, data_length + term_len);

    // now we wait for the response
    // the challenge is that this can vary depending on the command
    std::unique_lock<std::mutex> ul(this->queue_response_mutex);

    while (this->queued_responses.size() == 0)
    {
        this->response_added.wait(ul);
    }

    auto message = this->queued_responses.front();
    this->queued_responses.pop_front();

    std::string message_text((char *)message.second.data(), message.second.size());

    bool success = false;

    if (starts_with(message_text, "OK") || starts_with(message_text, "[*]OK"))
    {
        success = true;
        std::cout << "Command successful.\n";
    }
    else if (starts_with(message_text, "ERROR"))
    {
        success = false;
        std::cerr << "Command failed:\n>" << message_text << "\n";
    }
    else if (starts_with(message_text, "BUSY"))
    {
        success = false;
        std::cout << "Modem Busy:\n>" << message_text << "\n";
    }
    else
    {
        // we don't want to get stuck here
        success = false;
        std::cerr << "Unknown response to command:\n>" << message_text << "\n"; 
    }

    return success;
}

bool EvologicsModem::send_mode(const char *d)
{
    size_t data_length = strlen(d);

    char *command = (char *)malloc(data_length + term_len);
    memcpy(command, d, data_length);
    memcpy(command + data_length, term, term_len);

    this->modem_write(command, data_length + term_len);

    // now we wait for the response
    // the challenge is that this can vary depending on the command
    std::unique_lock<std::mutex> ul(this->queue_response_mutex);

    while (this->queued_responses.size() == 0)
    {
        this->response_added.wait(ul);
    }

    auto message = this->queued_responses.front();
    this->queued_responses.pop_front();

    std::string message_text((char *)message.second.data(), message.second.size());

    bool success = false;

    if (starts_with(message_text, "INITIATION"))
    {
        success = true;
        std::cout << "Command successful:\n" << message_text << "\n";
    }
    else if (starts_with(message_text, "ERROR"))
    {
        success = false;
        std::cerr << "Command failed:\n>" << message_text << "\n";
    }
    else if (starts_with(message_text, "BUSY"))
    {
        success = false;
        std::cout << "Modem Busy:\n>" << message_text << "\n";
    }
    else
    {
        // we don't want to get stuck here
        success = false;
        std::cerr << "Unknown response to command:\n>" << message_text << "\n"; 
    }

    return success;
}

bool EvologicsModem::configure_modem()
{
    char cmd[128];

    if (!send_command("AT@CTRL"))
        return false;
	
    snprintf(cmd, sizeof(cmd), "AT!L%d", source_level);
    if (!send_command(cmd))
        return false;

    snprintf(cmd, sizeof(cmd), "AT!G%d", gain);
    if (!send_command(cmd))
        return false;

    snprintf(cmd, sizeof(cmd), "AT!CA%d", sound_speed);
    if (!send_command(cmd))
        return false;

    if(auto_gain)
    {
        if (!send_command("AT!LC1"))
            return false;
    }
    else
    {
        if (!send_command("AT!LC0"))
            return false;
    }

    // Get the local address
    if (!send_query("AT?AL"))
        return false;

    // get the general status
    if (!send_query("AT?S"))
        return false;

    // set LISTEN mode
    /*if (!send_mode("ATA"))
        return false;*/

    // request USBL positioning data
    if (!send_command("AT@ZU1"))
        return false;     

    // request extended notifications
    if (!send_command("AT@ZX1"))
        return false;
    
    // set the retry timeout on burst data
    if (!send_command("AT!RT1500"))
        return false;

    // set the retry count on burst data
    if (!send_command("AT!RC1"))
        return false;

    // set max address to 14
    if (!send_command("AT!AM14"))
        return false;

    return true;
}

int EvologicsModem::get_target_channel(std::string const &channel)
{
    int channel_pos = channel.find_last_of('.');
    std::string target_name = channel.substr(channel_pos + 1);

    int target_channel = 0;
    // see if we know it from the ping targets
    // could also have a separate set of mappings here
    for (auto &target : ping_targets)
    {
        if (target.target_name == target_name)
        {
            target_channel = target.target_id;
            break;
        }
    }

    if (target_channel == 0)
    {
        target_channel = this->topside_id;
    }


    return target_channel;
}

void EvologicsModem::on_heartbeat(const lcm::ReceiveBuffer* rbuf, const std::string &channel, const perllcm::heartbeat_t *hb)
{
    std::cout << "Thump" << std::endl;
    // heartbeat! we need to see if we have to ping someone
    // first check if we have a ping queued. if we do, then do nothing

    if (this->next_ping.size() > 0)
    {
        // have a queued ping. don't replace it
        return;
    }

    // what is our highest priority target to ping?
    // a target is immediately ignored if we have sent a fix too recently
    // (we base on this not pings in case extra messages have been sent to it
    // and given us a fix in the interim)
    // we also discard any within the minimum ping period
    // out of those that are left we pick the oldest ping time
    int target = 0;
    int64_t now = timestamp_now();
    int64_t last_ping_time = now;

    for (auto &pt : this->ping_targets)
    {
        int64_t cutoff = now - pt.minimum_period;
        if (pt.last_sent_fix_time < cutoff && pt.last_ping_time < cutoff)
        {
            if (pt.last_ping_time < last_ping_time)
            {
                last_ping_time = pt.last_ping_time;
                target = pt.target_id;
            }
        }
    }

    // if we have a viable target
    if (target != 0)
    {
        std::lock_guard<std::mutex> lg(this->queue_message_mutex);

        // build the message up
        char message[64];
        int length = snprintf(message, sizeof(message), "AT*SENDIM,5,%d,ack,%05d", target, target);

        // and place in the queue
        next_ping.resize(length);
        memcpy(next_ping.data(), message, length);

        // also don't forget to update the ping time
        for (auto &pt : this->ping_targets)
        {
            if (pt.target_id == target)
            {
                pt.last_ping_time = now;
            }
        }
    }

}

void EvologicsModem::on_evo_control(const lcm::ReceiveBuffer* rbuf, const std::string &channel, const senlcm::evologics_command_t *ec)
{
    // disable sending fixes etc.
    std::cout << "Implement/Update EVO control." << std::endl;
}

void EvologicsModem::on_lcm_data(const lcm::ReceiveBuffer* rbuf, const std::string &channel)
{
    // queue the message for sending, don't just fire it off
    int target_channel = get_target_channel(channel);

    std::vector<unsigned char> message = this->build_lcm_data_message((uint8_t *)rbuf->data, rbuf->data_size, target_channel, channel.c_str(), false);

    // now to queue the message
    std::lock_guard<std::mutex> lg(this->queue_message_mutex);

    // TODO: this should really know if an IM, IMS or BURST.
    // but in practice we are sending IM or BURST only and they behave similarly
    // with regards to delivery reports
    this->queued_messages.emplace_back(MSG_IM, std::vector<uint8_t>());
    this->queued_messages.back().second.swap(message);

    this->message_added.notify_one();
}

void EvologicsModem::on_lcm_pbm_data(const lcm::ReceiveBuffer* rbuf, const std::string &channel)
{
    // queue the message for sending, don't just fire it off
    int target_channel = get_target_channel(channel);
    std::vector<unsigned char> message = this->build_lcm_data_message((uint8_t *)rbuf->data, rbuf->data_size, target_channel, channel.c_str(), true);

    this->send_message(MSG_PBM, (char *)message.data(), message.size());
}

void EvologicsModem::on_usbl_fix(const lcm::ReceiveBuffer* rbuf, const std::string &channel)
{
    int target = get_target_channel(channel);

    int64_t now = timestamp_now();

    bool send_fix = false;

    // check if we need to send to this target
    for (auto &pt : this->ping_targets)
    {
        if (pt.target_id == target)
        {
            if (pt.send_fixes && pt.last_sent_fix_time < now - pt.minimum_period)
            {
                send_fix = true;
                pt.last_sent_fix_time = now;
            }
            break;
        }
    }

    if (send_fix)
    {
        std::vector<unsigned char> message = this->build_lcm_data_message((uint8_t *)rbuf->data, rbuf->data_size, target, channel.c_str(), false);

        // now to queue the message
        std::lock_guard<std::mutex> lg(this->queue_message_mutex);

        // TODO: this should really know if an IM, IMS or BURST.
        // but in practice we are sending IM or BURST only and they behave similarly
        // with regards to delivery reports
        this->queued_messages.emplace_back(MSG_IM, std::vector<uint8_t>());
        this->queued_messages.back().second.swap(message);

        this->message_added.notify_one();
    }
}

std::vector<unsigned char>
EvologicsModem::build_lcm_data_message
    (
        unsigned char *lcm_data,
        int lcm_data_size,
        int target_id,
        const char *dest_channel,
        bool use_pbm
    )
{
    int message_size = lcm_data_size + strlen(dest_channel) + 10;

    char header[64];
    int header_size = 0;
    if (message_size <= 64)
    {
        if (use_pbm)
        {
            header_size = snprintf(header, sizeof(header), "AT*SENDPBM,%d,%d,", message_size, target_id);
        }
        else
        {
            header_size = snprintf(header, sizeof(header), "AT*SENDIM,%d,%d,ack,", message_size, target_id);
        }
    }
    else if (use_pbm)
    {
        std::cerr << "Tried to send PBM, but data too large." << std::endl;
        return std::vector<unsigned char>();
    }
    else
    {
        // send as burst
        header_size = snprintf(header, sizeof(header), "AT*SEND,%d,%d,", message_size, target_id);
    }

    int bufpos = 0;
    std::vector<unsigned char> buffer(header_size + message_size);
    char *buf = (char *)buffer.data();

    memcpy(buf, header, header_size);
    bufpos += header_size;

    // prefix LCM <length of channel name> <channel name> <data>
    strncpy(buf, "LCM", 3);
    bufpos += 3;

    // TODO: investigate using lookups for this
    buf[bufpos] = strlen(dest_channel);
    bufpos += 1;

    memcpy(buf + bufpos, dest_channel, strlen(dest_channel));
    bufpos += strlen(dest_channel);

    memcpy(buf + bufpos, lcm_data, lcm_data_size);
    bufpos += lcm_data_size;

    uint32_t crc = crc32(0, (unsigned char *)buf + 3, lcm_data_size + strlen(dest_channel) + 1);
    memcpy(buf + bufpos, &crc, sizeof(crc));
    bufpos += sizeof(crc);

    // TODO: remove tail flag - not needed with updated parsing/state code
    strncpy(buf + bufpos, "LE", 2);
    bufpos += 2;

    if (bufpos != header_size + message_size)
    {
        std::cerr << "Error in encoding LCM message. Data size mismatch." << std::endl;
    }

    return buffer;
}

bool EvologicsModem::send_message(int message_type, char const *data, int length)
{
    // now send the message
    this->modem_write(data, length);

    // to this we expect a few things
    // namely an OK, BUSY or ERROR response
    // followed by a final message depending on the message type
    std::unique_lock<std::mutex> ul(this->queue_response_mutex);

    while (this->queued_responses.size() == 0)
    {
        std::cout << "Waiting for SEND response" << std::endl;
        this->response_added.wait_for(ul, std::chrono::milliseconds(100));
    }

    auto message = this->queued_responses.front();
    this->queued_responses.pop_front();

    std::string message_text((char *)message.second.data(), message.second.size());
    bool success;

    if (starts_with(message_text, "OK") || starts_with(message_text, "[*]OK"))
    {
        std::cout << "Message sent.\n";
        success = true;
    }
    else if (starts_with(message_text, "ERROR"))
    {
        std::cerr << "Command failed:\n>" << message_text << "\n";
        success = false;
    }
    else if (starts_with(message_text, "BUSY"))
    {
        success = false;
        std::cout << "Modem Busy:\n>" << message_text << "\n";
    }
    else
    {
        // we don't want to get stuck here
        success = false;
        std::cerr << "Unknown response to command:\n>" << message_text << "\n"; 
    }

    // we get feedback as long as it isn't a PBM
    // (or an IMS - they don't give delivery reports but will say if cancelled/expired
    // which bypasses the response queue)
    // (all the IMs we send currently require ack note that
    // this will change if using broadcast IMs)
    if (success && (message_type == MSG_BURST || message_type == MSG_IM))
    {
        // now was it delivered, failed or cancelled?
        // this also depends on the message type
        while (this->queued_responses.size() == 0)
        {
            std::cout << "Waiting for SEND[IM] delivery response" << std::endl;
            this->response_added.wait_for(ul, std::chrono::milliseconds(100));
        }

        message = this->queued_responses.front();
        this->queued_responses.pop_front();

        std::string message_text((char *)message.second.data(), message.second.size());

        if (starts_with(message_text, "DELIVERED"))
        {
            success = true;
            std::cout << "Message delivered.\n";
        }
        else if (starts_with(message_text, "CANCELLED"))
        {
            success = false;
            std::cerr << "Message cancelled.\n";
        }
        else if (starts_with(message_text, "FAILED"))
        {
            success = false;
            std::cout << "Message failed.\n";
        }
        else
        {
            // we don't want to get stuck here
            success = false;
            std::cerr << "Unknown response to command:\n>" << message_text << "\n"; 
        }
    }


    return success;
}


void EvologicsModem::lcm_handle_thread()
{
    while (!loop_exit)
    {
        this->lcm.handleTimeout(100);
    }

    std::cout << "LCM handle thread exit" << std::endl;
}

void EvologicsModem::modem_read_thread()
{
    fd_set rfds;
    std::vector<uint8_t> buf;
    //buf.reserve(1024);
    size_t bytes;
    int64_t timestamp;
    
    while(!loop_exit)
    {
        FD_ZERO (&rfds);
        FD_SET (this->modem_fd, &rfds);
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int ret = select (FD_SETSIZE, &rfds, NULL, NULL, &timeout);
        timestamp = timestamp_now();
        if(ret > 0)
        {
            bytes = 0;
            buf.clear();
            // the cases we are facing for decoding messages
            // include RECV* messages - some of which contain binary
            // data that could accidentally match the line termination
            // characters (CR + LF)
            //
            // the shortest possible message is 4 bytes - OK\r\n
            // which coincidentally matches the length needed to get
            // RECV matched and passed to a special handler for those messages
            // so start with 4 bytes!
            // make sure we get exactly 4 bytes (in a row)
            uint8_t message_start[4];
            while (bytes < 2)
            {
                bytes += read(this->modem_fd, message_start + bytes, 4 - bytes);
                // in case the modem sends a blank response...
                // it happens (with AT@ZU1 apparently...)
                if (message_start[0] == '\r')
                {
                    bytes = 0;
                }
            }
            while (bytes < 4)
            {
                bytes += read(this->modem_fd, message_start + bytes, 4 - bytes);
            }

            // faster than looping
            buf.push_back(message_start[0]);
            buf.push_back(message_start[1]);
            buf.push_back(message_start[2]);
            buf.push_back(message_start[3]);


            // now we can check if it is a recv
            if (buf[0] == 'R' && buf[1] == 'E' && buf[2] == 'C' && buf[3] == 'V')
            {
                this->parse_recv(buf);
            }
            else
            {
                this->parse_generic(buf);
            }

            this->publish_modem_response(timestamp, buf);
            this->queue_modem_response(timestamp, buf);
        }
        else if (ret == 0)
        {
            std::cout << "Timeout waiting for modem data.\n";
        }
        else if (ret == -1)
        {
            char tmpbuf[64];
            sprintf(tmpbuf, "Select failed on read thread %d", this->modem_fd);
            perror(tmpbuf);
            close(this->modem_fd);
            this->connect_modem();
        }
    }

    std::cout << "Read thread exit\n";
}


void EvologicsModem::parse_recv(std::vector<uint8_t> &buf)
{
    size_t bytes = buf.size();

    if (bytes != 4)
    {
        std::cerr << "Should only be parsing RECV commands starting with 4 bytes." << std::endl;
    }

    // we need to get the length of the data at the end
    // and also need to find out what type of RECV message it is.
    uint8_t byte;
    bytes += read(this->modem_fd, &byte, 1);
    buf.push_back(byte);

    size_t field;
    switch (byte)
    {
        case 'S':
        case 'E':
            // RECVSTART
            // RECVEND
            this->parse_generic(buf);
            return;
        case 'P':
            // RECVPBM
            field = 8;
        default:
            // RECV
            // RECVIM
            // RECVIMS
            field = 9;
    }


    // this is straightforward
    // read until 'field' commas have been read in
    // ascii to integer the number between 
    size_t comma_count = 0;
    if (buf.back() == ',')
    {
        comma_count++;
    }

    while (comma_count < field)
    {
        bytes += read(this->modem_fd, &byte, 1);
        buf.push_back(byte);
        if (byte == ',')
        {
            comma_count++;
        }
    }

    // get the characters between the first two commas
    // then read it in
    size_t data_length = atoi(strchr((char *)buf.data(), ',') + 1);

    // two options
    // first is read each byte in a loop, second is allocate dynamically and loop through

    uint8_t *buffer = (uint8_t *)malloc(data_length);
    size_t data_read = 0;
    while (data_read < data_length)
    {
        data_read += read(this->modem_fd, buffer + data_read, data_length - data_read);
    }

    for (size_t ii=0; ii < data_length; ++ii)
    {
        buf.emplace_back(buffer[ii]);
    }
    free(buffer);

    // at this point should be two more bytes and done
    // let the main routine handle it
    this->parse_generic(buf);
}

void EvologicsModem::parse_generic(std::vector<uint8_t> &buf)
{
    size_t bytes = buf.size();
    while(true)
    {
        // We are coming in with already loaded data so need to check first
        // if we have a message end. We can safely just check for end of
        // response as the only responses with freeform data are parsed
        // in a different function
        if(bytes > 1 && ((buf[bytes-2] == 0x0D) && (buf[bytes-1] == 0x0A)))
        {
            buf.push_back(0); // add null termination
            break;
        }

        uint8_t byte;
        bytes += read(this->modem_fd, &byte, 1);
        buf.push_back(byte);

    }
}

void EvologicsModem::publish_modem_response(int64_t timestamp, std::vector<uint8_t> const &buf)
{
    //TODO: update this to display all messages excluding any data fields
    // (this includes many RECV and SEND cmds, but not all!)
    //std::cout << "Received from modem:\n>>" << 
    std::cout << ">>" << 
        std::string((char *)buf.data(), strcspn((char *)buf.data(), ",\n")) << std::endl;
    senlcm::evologics_modem_t msg;
    msg.utime = timestamp;
    msg.size = buf.size() - 1;
    msg.data = buf;

    char channel_name[128];
    snprintf(channel_name, 128, "%s.EVOLOGICS_LOG", vehicle_name.c_str());
    this->lcm.publish(channel_name, &msg);
}

void EvologicsModem::queue_modem_response(int64_t timestamp, std::vector<uint8_t> &buffer)
{

    // here is a slight challenge
    // we want to queue anything that needs a response from the system
    // and just output everything else
    // so if the response is OK, ERROR, BUSY etc. we queue
    // as well as some other specific cases
    std::string response((char *)buffer.data(), buffer.size());

    // we never need to respond to these
    // they have already been logged so move on!
    if (starts_with(response, "RECV"))
    {
        if (starts_with(response, "RECV,"))
        {
            this->process_burst(timestamp, buffer);
        }
        else if (starts_with(response, "RECVIM,"))
        {
            this->process_im(timestamp, buffer);
        }
        else if (starts_with(response, "RECVPBM,"))
        {
            this->process_pbm(timestamp, buffer);
        }

        return;
    }
    else if (starts_with(response, "SEND"))
    {
        return;
    }
    else if (starts_with(response, "USBL"))
    {
        if (starts_with(response, "USBLLONG,"))
        {
            this->process_usbllong(timestamp, buffer);
        }
        else if (starts_with(response, "USBLANGLES,"))
        {
            this->process_usblangles(timestamp, buffer);
        }
        return;
    }
    else if (starts_with(response, "PHYO"))
    {
        return;
    }
    else if (starts_with(response, "BITRATE"))
    {
        return;
    }
    else if (starts_with(response, "SRCLEVEL"))
    {
        return;
    }
    else if (starts_with(response, "EXPIREDIMS"))
    {
        std::cerr << "IMS expired." << std::endl;
        return;
    }
    else if (starts_with(response, "CANCELLEDIMS"))
    {
        std::cerr << "IMS expired." << std::endl;
        return;
    }
    else if (starts_with(response, "CANCELLEDPBM"))
    {
        std::cerr << "IMS expired." << std::endl;
        return;
    }

    /*
    // check for any responses that we need to know
    // a command worked
    bool queue = false;
    if (response.find("OK") == 0)
    {
        queue = true;
    }
    else if (response.find("ERROR") == 0)
    {
        queue = true;
    }
    else if (response.find("BUSY") == 0)
    {
        queue = true;
    }
    else if (response.find("DELIVERED") == 0)
    {
        queue = true;
    }
    else if (response.find("FAILED") == 0)
    {
        queue = true;
    }
    else if (response.find("CANCELLED") == 0)
    {
        queue = true;
    }
    */

    // for now assume we queue everything that isn't
    // a message received/extended notifications response
    if (true) // (queue)
    {
        std::lock_guard<std::mutex> lg(this->queue_response_mutex);

        // now we can add into the array!
        std::vector<uint8_t> new_buffer;

        new_buffer.swap(buffer);

        this->queued_responses.emplace_back(timestamp, new_buffer);

        this->response_added.notify_one();
    }
}

void EvologicsModem::process_lcm_data(uint8_t *d, int size)
{
    // first check the crc
    unsigned long crc = crc32(0, d + 3, size - 9);
    unsigned long data_crc = *(uint32_t *)&d[size - 6];
    
    if((data_crc & 0xFFFFFFFF) != (crc & 0xFFFFFFFF))
    {
        std::cerr << "LCM data CRC error\n";
        //printf("0x%X 0x%X\n", crc, data_crc);
        return;
    }

    // get the channel name
    char *channel = (char *)malloc(d[3] + 1);
    memset(channel, 0, d[3] + 1);
    memcpy(channel, &d[4], d[3]);

    // get the payload location
    void *lcm_data_start = &d[d[3] + 4];
    int lcm_data_length = size - d[3] - 10;
    
    std::cout << "Publishing lcm data on channel name: " << channel << std::endl;
    
    // publish the LCM data
    lcm.publish(channel, lcm_data_start, lcm_data_length);
}


void EvologicsModem::run()
{
    // first step is make sure we are connected!
    while (!this->connect_modem())
    {
        std::cerr << "Failed to connect to modem. Trying again." << std::endl;
    }

    std::cout << "Modem connected." << std::endl;

    // spin off the thread to read from the modem
    // it will queue to a list of responses that we need
    // both generally and whilst configuring
    std::thread read_thread(&EvologicsModem::modem_read_thread, this);
    std::thread handle_thread(&EvologicsModem::lcm_handle_thread, this);

    // and configured!
    while (!this->configure_modem())
    {
        std::cerr << "Failed to configure modem. Trying again." << std::endl;
    }
    std::cout << "Modem configured." << std::endl;

    while (!loop_exit)
    {
        std::unique_lock<std::mutex> ul(this->queue_message_mutex);
        while (this->queued_messages.size() == 0 && this->next_ping.size() == 0)
        {
            std::cout << "Waiting for message to send." << std::endl;
            this->message_added.wait_for(ul, std::chrono::milliseconds(500));
        }

        if (this->queued_messages.size() > 0)
        {
            // sending burst/data IM
            auto message = this->queued_messages.front();
            this->queued_messages.pop_front();

            send_message(message.first, (char *)message.second.data(), message.second.size());
        }
        else if (this->next_ping.size() > 0)
        {
            // sending a ping for a USBL fix
            send_message(MSG_IM, (char *)next_ping.data(), next_ping.size());
            next_ping.clear();
        }
    }

    read_thread.join();
    handle_thread.join();
}

int main(int argc, char **argv)
{
    loop_exit = 0;
    if (signal(SIGINT, signal_handler) == SIG_ERR)
         perror("ERROR: Can't register signal_handler for SIGINT");
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
         perror("ERROR: Can't register signal_handler for SIGPIPE");
    EvologicsModem *modem = new EvologicsModem();

    modem->load_configuration(basename((argv[0])));

    modem->run();

    delete modem;

    return 0;
}
