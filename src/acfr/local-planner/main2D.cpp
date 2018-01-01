#include <unistd.h>
#include "local_planner2D.hpp"

void
print_help (int exval, char **argv)
{
    printf("Usage:%s [-h] [-n VEHICLE_NAME]\n\n", argv[0]);

    printf("  -h                               print this help and exit\n");
    printf("  -n VEHICLE_NAME                  set the vehicle_name\n");
    exit (exval);
}

void
parse_args (int argc, char **argv, LocalPlanner2D *lp)
{
    int opt;

    while ((opt = getopt (argc, argv, "hn:")) != -1)
    {
        switch(opt)
        {
        case 'h':
            print_help (0, argv);
            break;
        case 'n':
            lp->setVehicleName((char*)optarg);
            break;
         }
    }
}

int main(int argc, char **argv)
{
	// install the signal handler

	LocalPlanner2D *lp = new LocalPlanner2D();
        parse_args(argc, argv, lp);
        lp->subscribeChannels();

	if (!lp->loadConfig(basename(argv[0])))
	{
		cerr << "Failed loading config" << endl;
		return 0;
	}

    // this is a blocking call that only returns when
    // the program signals an exit condition
	lp->process();

	delete lp;

	return 0;
}

