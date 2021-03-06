package pilot_gui;
import lcm.lcm.*;
import java.util.Scanner;
import java.io.*;
import java.lang.Math;
import hauv.pl_raw_t;
import hauv.pl_san_t;
import hauv.pl_ghp_t;
import hauv.pl_gbp_t;
import hauv.pl_rbo_t;
import hauv.bs_nvg_t;
import hauv.bs_rnv_t;
import hauv.bs_dvl_t;
import hauv.vehicle_state_t;
import hauv.vehicle_plan_t;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * PilotGUI.java
 *
 * Created on Oct 5, 2009, 1:51:15 PM
 */

//package pilot;

/**
 *
 * @author benglot
 */
public class PilotGUIRectangle extends javax.swing.JFrame {

    //Declare a few variables
    // Thread t = null;
    LCM myLCM = LCM.getSingleton();
    //static ReceiveRNV rnv = new ReceiveRNV();
    //static ReceiveNVG nvg = new ReceiveNVG();
    //public static int flag = 0;
    //public static int clickCount = 0;
    //public double reset = 0;
    //public double wayptCount = 0;
    //public double vehLocation = 0;

    static double width = 0;
    static double length = 0;
    static double des_depth = 0; //Edit as desired depending on operating environment
    static double heading_cmd = 0;

    static boolean ulb;
    static boolean urb;
    static boolean lrb;
    static boolean llb;
    static boolean cwb;
    static boolean ccwb;

    ///////New variables for the PLAN message/////////
    
    static int npoints = 4; //Size is unique to this particular class
    static double[][] waypts = new double[npoints][2]; //Size is unique to this particular class
    static boolean stop = false;
    static boolean heading = false;
    static boolean holdStation = false;
    static boolean broadcast = false;

    /** Creates new form PilotGUI */
    public PilotGUIRectangle() {
        initComponents();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        VehLocationButtonGroup1 = new javax.swing.ButtonGroup();
        DirectionButtonGroup = new javax.swing.ButtonGroup();
        UpperLeftRadioButton = new javax.swing.JRadioButton();
        UpperRightRadioButton = new javax.swing.JRadioButton();
        LowerLeftRadioButton = new javax.swing.JRadioButton();
        LowerRightRadioButton = new javax.swing.JRadioButton();
        BroadcastButton = new javax.swing.JButton();
        StopButton = new javax.swing.JButton();
        LengthField = new javax.swing.JTextField();
        WidthField = new javax.swing.JTextField();
        WidthLabel = new javax.swing.JLabel();
        LengthLabel = new javax.swing.JLabel();
        ClockwiseButton = new javax.swing.JRadioButton();
        CounterclockwiseButton = new javax.swing.JRadioButton();
        DepthField = new javax.swing.JTextField();
        DepthLabel = new javax.swing.JLabel();
        HeadingField = new javax.swing.JTextField();
        HeadingButton = new javax.swing.JButton();
        HoldStationButton = new javax.swing.JToggleButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("HAUV Pilot Planner");

        VehLocationButtonGroup1.add(UpperLeftRadioButton);
        UpperLeftRadioButton.setSelected(true);
        UpperLeftRadioButton.setText("HAUV Location");
        UpperLeftRadioButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    UpperLeftRadioButtonActionPerformed(evt);
                }
            });

        VehLocationButtonGroup1.add(UpperRightRadioButton);
        UpperRightRadioButton.setText("HAUV Location");
        UpperRightRadioButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    UpperRightRadioButtonActionPerformed(evt);
                }
            });

        VehLocationButtonGroup1.add(LowerLeftRadioButton);
        LowerLeftRadioButton.setText("HAUV Location");
        LowerLeftRadioButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    LowerLeftRadioButtonActionPerformed(evt);
                }
            });

        VehLocationButtonGroup1.add(LowerRightRadioButton);
        LowerRightRadioButton.setText("HAUV Location");
        LowerRightRadioButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    LowerRightRadioButtonActionPerformed(evt);
                }
            });

        BroadcastButton.setText("Broadcast!");
        BroadcastButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    BroadcastButtonActionPerformed(evt);
                }
            });

        StopButton.setText("Stop!");
        StopButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    StopButtonActionPerformed(evt);
                }
            });

        LengthField.setText("0");
        LengthField.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    LengthFieldActionPerformed(evt);
                }
            });

        WidthField.setText("0");
        WidthField.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    WidthFieldActionPerformed(evt);
                }
            });

        WidthLabel.setText("Width");

        LengthLabel.setText("Length");

        DirectionButtonGroup.add(ClockwiseButton);
        ClockwiseButton.setSelected(true);
        ClockwiseButton.setText("Clockwise");
        ClockwiseButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    ClockwiseButtonActionPerformed(evt);
                }
            });

        DirectionButtonGroup.add(CounterclockwiseButton);
        CounterclockwiseButton.setText("Counterclockwise");
        CounterclockwiseButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    CounterclockwiseButtonActionPerformed(evt);
                }
            });

        DepthField.setText("0");
        DepthField.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    DepthFieldActionPerformed(evt);
                }
            });

        DepthLabel.setText("Depth");

        HeadingField.setText("0");
        HeadingField.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    HeadingFieldActionPerformed(evt);
                }
            });

        HeadingButton.setText("Adjust Heading");
        HeadingButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    HeadingButtonActionPerformed(evt);
                }
            });

        HoldStationButton.setText("Hold Station");
        HoldStationButton.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    HoldStationButtonActionPerformed(evt);
                }
            });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
                                  layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                  .addGroup(layout.createSequentialGroup()
                                            .addContainerGap()
                                            .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                                      .addGroup(layout.createSequentialGroup()
                                                                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                                                          .addGroup(layout.createSequentialGroup()
                                                                                    .addComponent(LengthField, javax.swing.GroupLayout.PREFERRED_SIZE, 42, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                                                    .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                                                                    .addComponent(LengthLabel))
                                                                          .addComponent(UpperLeftRadioButton))
                                                                .addGap(18, 18, 18)
                                                                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                                                          .addGroup(layout.createSequentialGroup()
                                                                                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                                                                              .addGroup(layout.createSequentialGroup()
                                                                                                        .addComponent(WidthField, javax.swing.GroupLayout.PREFERRED_SIZE, 43, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                                                                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                                                                                        .addComponent(WidthLabel))
                                                                                              .addGroup(layout.createSequentialGroup()
                                                                                                        .addComponent(DepthField, javax.swing.GroupLayout.PREFERRED_SIZE, 42, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                                                                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                                                                                        .addComponent(DepthLabel)))
                                                                                    .addGap(53, 53, 53)
                                                                                    .addComponent(UpperRightRadioButton))
                                                                          .addComponent(ClockwiseButton)
                                                                          .addComponent(CounterclockwiseButton)))
                                                      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                                                                .addComponent(LowerLeftRadioButton)
                                                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 164, Short.MAX_VALUE)
                                                                .addComponent(LowerRightRadioButton))
                                                      .addGroup(layout.createSequentialGroup()
                                                                .addGap(126, 126, 126)
                                                                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                                                          .addGroup(layout.createSequentialGroup()
                                                                                    .addComponent(BroadcastButton)
                                                                                    .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                                                                    .addComponent(StopButton, javax.swing.GroupLayout.PREFERRED_SIZE, 68, javax.swing.GroupLayout.PREFERRED_SIZE))
                                                                          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                                                                                    .addComponent(HoldStationButton, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                                                                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                                                                                              .addComponent(HeadingField, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                                                              .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                                                                              .addComponent(HeadingButton))))))
                                            .addContainerGap())
                                  );
        layout.setVerticalGroup(
                                layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                .addGroup(layout.createSequentialGroup()
                                          .addContainerGap()
                                          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                                    .addGroup(layout.createSequentialGroup()
                                                              .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                                                        .addComponent(UpperLeftRadioButton)
                                                                        .addComponent(UpperRightRadioButton))
                                                              .addGap(33, 33, 33)
                                                              .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                                                        .addComponent(LengthField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                                        .addComponent(LengthLabel)))
                                                    .addGroup(layout.createSequentialGroup()
                                                              .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                                                        .addComponent(WidthField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                                        .addComponent(WidthLabel))
                                                              .addGap(18, 18, 18)
                                                              .addComponent(ClockwiseButton)
                                                              .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                                              .addComponent(CounterclockwiseButton)
                                                              .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                                              .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                                                        .addComponent(DepthField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                                        .addComponent(DepthLabel))))
                                          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                                    .addComponent(LowerRightRadioButton)
                                                    .addComponent(LowerLeftRadioButton))
                                          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                                    .addComponent(BroadcastButton)
                                                    .addComponent(StopButton))
                                          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                          .addComponent(HoldStationButton)
                                          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 9, Short.MAX_VALUE)
                                          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                                    .addComponent(HeadingField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                                    .addComponent(HeadingButton))
                                          .addContainerGap())
                                );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    /*    public void run() {

	//Read in the width and length fields
	width = Double.parseDouble(WidthField.getText());
	length = Double.parseDouble(LengthField.getText());


    //	if (clickCount == 1) { //Start the listener only once
    //	    myLCM.subscribe("HAUV_BS_RNV", rnv);
    //	    myLCM.subscribe("HAUV_VEHICLE_STATE", nvg);
    //	}
    //	System.out.println("ULRB: " + ulb + 
    //			   " CB: " + cwb +
    //			   " width: " + width +
    //			   " height: " + length);


	//Figure out which corner contains the HAUV
	//double waypts[][] = new double[4][2];
	if (ulb) {
    if (cwb) {
    //double[][] waypts = {{0,0},{0,width},{-length,width},{-length,0}};
    waypts[1][1] = width; 
    waypts[2][0] = -length; 
    waypts[2][1] = width; 
    waypts[3][0] = -length;
    }
    else {
    //double[][] waypts = {{0,0},{-length,0},{-length,width},{0,width}};
    waypts[1][0] = -length;
    waypts[2][0] = -length;
    waypts[2][1] = width;
    waypts[3][1] = width;
    }
	}
	else if (urb) {
    if (cwb) {
    //double[][] waypts = {{0,0},{-length,0},{-length,-width},{0,-width}};
    waypts[1][0] = -length;
    waypts[2][0] = -length;
    waypts[2][1] = -width;
    waypts[3][1] = -width;
    }
    else {
    //double[][] waypts = {{0,0},{0,-width},{-length,-width},{-length,0}};
    waypts[1][1] = -width;
    waypts[2][0] = -length;
    waypts[2][1] = -width;
    waypts[3][0] = -length;
    }
	}
	else if (lrb) {
    if (cwb) {
    //double[][] waypts = {{0,0},{0,-width},{length,-width},{length,0}};
    waypts[1][1] = -width;
    waypts[2][0] = length;
    waypts[2][1] = -width;
    waypts[3][0] = length;
    }
    else {
    //double[][] waypts = {{0,0},{length,0},{length,-width},{0,-width}};
    waypts[1][0] = length;
    waypts[2][0] = length;
    waypts[2][1] = -width;
    waypts[3][1] = -width;
    }
	}
	else if (llb) {
    if (cwb) {
    //double[][] waypts = {{0,0},{length,0},{length,width},{0,width}};
    waypts[1][0] = length;
    waypts[2][0] = length;
    waypts[2][1] = width;
    waypts[3][1] = width;
    }
    else {
    //double[][] waypts = {{0,0},{0,width},{length,width},{length,0}};
    waypts[1][1] = width;
    waypts[2][0] = length;
    waypts[2][1] = width;
    waypts[3][0] = length;
    }

    System.out.println(waypts[0][0]+" "+waypts[0][1]+" "+waypts[1][0]+" "+waypts[1][1]+" "+waypts[2][0]+" "+waypts[2][1]+" "+waypts[3][0]+" "+waypts[3][1]);


	}
	/////////////////////////////////////////////////
	// publish a PLAN message with the new waypoints (maybe publish a few times to ensure the listener gets it)
	////////////////////////////////////////////////
	//Issue the waypoint command using backseat driver
	hauv.vehicle_plan_t plan = new hauv.vehicle_plan_t();
	plan.time = System.nanoTime();	    	
	plan.stop = stop;
	plan.holdStation = holdStation;
	plan.broadcast = broadcast;
	plan.heading = heading;
	plan.headingOffset = heading_cmd;
	plan.npoints = npoints;
	plan.waypoints = waypts;
	plan.depth = des_depth;	
	myLCM.publish ("HAUV_VEHICLE_PLAN", plan);

	try {
	//Delay for () milliseconds
	Thread.sleep(100); 
	} catch (InterruptedException ex) {
	} 

	broadcast = false;
	plan.broadcast = broadcast;
	myLCM.publish("HAUV_VEHICLE_PLAN", plan);


    } */


    private void UpperLeftRadioButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_UpperLeftRadioButtonActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_UpperLeftRadioButtonActionPerformed

    private void UpperRightRadioButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_UpperRightRadioButtonActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_UpperRightRadioButtonActionPerformed

    private void LowerLeftRadioButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_LowerLeftRadioButtonActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_LowerLeftRadioButtonActionPerformed

    private void LowerRightRadioButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_LowerRightRadioButtonActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_LowerRightRadioButtonActionPerformed

    private void WidthFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_WidthFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_WidthFieldActionPerformed

    private void LengthFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_LengthFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_LengthFieldActionPerformed

    private void ClockwiseButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ClockwiseButtonActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_ClockwiseButtonActionPerformed

    private void CounterclockwiseButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_CounterclockwiseButtonActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_CounterclockwiseButtonActionPerformed

    private void DepthFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_DepthFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_DepthFieldActionPerformed

    private void BroadcastButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_BroadcastButtonActionPerformed
        // TODO add your handling code here:

        broadcast = true;

        //Read in the width and length fields
        width = Double.parseDouble(WidthField.getText());
        length = Double.parseDouble(LengthField.getText());
        des_depth = Double.parseDouble(DepthField.getText());
        ulb = UpperLeftRadioButton.isSelected();
        urb = UpperRightRadioButton.isSelected();
        lrb = LowerRightRadioButton.isSelected();
        llb = LowerLeftRadioButton.isSelected();
        cwb = ClockwiseButton.isSelected();
        ccwb = CounterclockwiseButton.isSelected();

        //Read in the width and length fields
        //	width = Double.parseDouble(WidthField.getText());
        //	length = Double.parseDouble(LengthField.getText());


        //	if (clickCount == 1) { //Start the listener only once
        //	    myLCM.subscribe("HAUV_BS_RNV", rnv);
        //	    myLCM.subscribe("HAUV_VEHICLE_STATE", nvg);
        //	}
        //	System.out.println("ULRB: " + ulb + 
        //			   " CB: " + cwb +
        //			   " width: " + width +
        //			   " height: " + length);

        waypts = new double[npoints][2];

        //Figure out which corner contains the HAUV
        //double waypts[][] = new double[4][2];
        if (ulb) {
            if (cwb) {
                //double[][] waypts = {{0,0},{0,width},{-length,width},{-length,0}};
                waypts[1][1] = width; 
                waypts[2][0] = -length; 
                waypts[2][1] = width; 
                waypts[3][0] = -length;
            }
            else {
                //double[][] waypts = {{0,0},{-length,0},{-length,width},{0,width}};
                waypts[1][0] = -length;
                waypts[2][0] = -length;
                waypts[2][1] = width;
                waypts[3][1] = width;
            }
        }
        else if (urb) {
            if (cwb) {
                //double[][] waypts = {{0,0},{-length,0},{-length,-width},{0,-width}};
                waypts[1][0] = -length;
                waypts[2][0] = -length;
                waypts[2][1] = -width;
                waypts[3][1] = -width;
            }
            else {
                //double[][] waypts = {{0,0},{0,-width},{-length,-width},{-length,0}};
                waypts[1][1] = -width;
                waypts[2][0] = -length;
                waypts[2][1] = -width;
                waypts[3][0] = -length;
            }
        }
        else if (lrb) {
            if (cwb) {
                //double[][] waypts = {{0,0},{0,-width},{length,-width},{length,0}};
                waypts[1][1] = -width;
                waypts[2][0] = length;
                waypts[2][1] = -width;
                waypts[3][0] = length;
            }
            else {
                //double[][] waypts = {{0,0},{length,0},{length,-width},{0,-width}};
                waypts[1][0] = length;
                waypts[2][0] = length;
                waypts[2][1] = -width;
                waypts[3][1] = -width;
            }
        }
        else if (llb) {
            if (cwb) {
                //double[][] waypts = {{0,0},{length,0},{length,width},{0,width}};
                waypts[1][0] = length;
                waypts[2][0] = length;
                waypts[2][1] = width;
                waypts[3][1] = width;
            }
            else {
                //double[][] waypts = {{0,0},{0,width},{length,width},{length,0}};
                waypts[1][1] = width;
                waypts[2][0] = length;
                waypts[2][1] = width;
                waypts[3][0] = length;
            }
        }

		System.out.println(waypts[0][0]+" "+waypts[0][1]+" "+waypts[1][0]+" "+waypts[1][1]+" "+waypts[2][0]+" "+waypts[2][1]+" "+waypts[3][0]+" "+waypts[3][1]);

        hauv.vehicle_plan_t plan = new hauv.vehicle_plan_t();
        plan.time = System.nanoTime();	    	
        plan.stop = stop;
        plan.broadcast = broadcast;
        plan.holdStation = holdStation;
        plan.heading = heading;
        plan.headingOffset = heading_cmd;
        plan.npoints = npoints;
        plan.waypoints = waypts;
        plan.depth = des_depth;	
        myLCM.publish ("HAUV_VEHICLE_PLAN", plan);

        try {
            //Delay for () milliseconds
            Thread.sleep(100); 
        } catch (InterruptedException ex) {
        } 

        broadcast = false;
        plan.broadcast = broadcast;
        myLCM.publish("HAUV_VEHICLE_PLAN", plan);


        //Start the thread which creates a new set of waypoints
        //	flag = 1;	
        //	PilotGUIRectangle pg = new PilotGUIRectangle();
        //	t = new Thread(pg);
        //	t.start();  

    }//GEN-LAST:event_BroadcastButtonActionPerformed

    private void StopButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_StopButtonActionPerformed
        // TODO add your handling code here:

        stop = true;
	
        hauv.vehicle_plan_t plan = new hauv.vehicle_plan_t();
        plan.time = System.nanoTime();	    	
        plan.stop = stop;
        plan.broadcast = broadcast;
        plan.holdStation = holdStation;
        plan.heading = heading;
        plan.headingOffset = heading_cmd;
        plan.npoints = npoints;
        plan.waypoints = waypts;
        plan.depth = des_depth;	
        myLCM.publish ("HAUV_VEHICLE_PLAN", plan);

        try {
            //Delay for () milliseconds
            Thread.sleep(100); 
        } catch (InterruptedException ex) {
        } 

        stop = false;
        plan.stop = stop;
        myLCM.publish("HAUV_VEHICLE_PLAN", plan);

    }//GEN-LAST:event_StopButtonActionPerformed

    private void HeadingFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_HeadingFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_HeadingFieldActionPerformed

    private void HeadingButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_HeadingButtonActionPerformed
        // TODO add your handling code here:

        // Publish a new PLAN message with the heading command (can send a few times if necessary)	
        heading_cmd = Double.parseDouble(HeadingField.getText())*Math.PI/180;
        heading = true;

        hauv.vehicle_plan_t plan = new hauv.vehicle_plan_t();
        plan.time = System.nanoTime();	    	
        plan.stop = stop;
        plan.broadcast = broadcast;
        plan.holdStation = holdStation;
        plan.heading = heading;
        plan.headingOffset = heading_cmd;
        plan.npoints = npoints;
        plan.waypoints = waypts;
        plan.depth = des_depth;	
        myLCM.publish ("HAUV_VEHICLE_PLAN", plan);

        try {
            //Delay for () milliseconds
            Thread.sleep(100); 
        } catch (InterruptedException ex) {
        } 

        heading = false;
        plan.heading = heading;
        myLCM.publish("HAUV_VEHICLE_PLAN", plan);


    }//GEN-LAST:event_HeadingButtonActionPerformed

    private void HoldStationButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_HoldStationButtonActionPerformed
        // TODO add your handling code here:

        holdStation = HoldStationButton.isSelected();

        hauv.vehicle_plan_t plan = new hauv.vehicle_plan_t();
        plan.time = System.nanoTime();	    	
        plan.stop = stop;
        plan.broadcast = broadcast;
        plan.holdStation = holdStation;
        plan.heading = heading;
        plan.headingOffset = heading_cmd;
        plan.npoints = npoints;
        plan.waypoints = waypts;
        plan.depth = des_depth;	
        myLCM.publish ("HAUV_VEHICLE_PLAN", plan);

    }//GEN-LAST:event_HoldStationButtonActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
                public void run() {
                    new PilotGUIRectangle().setVisible(true);
                }
            });
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton BroadcastButton;
    private javax.swing.JRadioButton ClockwiseButton;
    private javax.swing.JRadioButton CounterclockwiseButton;
    private javax.swing.JTextField DepthField;
    private javax.swing.JLabel DepthLabel;
    private javax.swing.ButtonGroup DirectionButtonGroup;
    private javax.swing.JButton HeadingButton;
    private javax.swing.JTextField HeadingField;
    private javax.swing.JToggleButton HoldStationButton;
    private javax.swing.JTextField LengthField;
    private javax.swing.JLabel LengthLabel;
    private javax.swing.JRadioButton LowerLeftRadioButton;
    private javax.swing.JRadioButton LowerRightRadioButton;
    private javax.swing.JButton StopButton;
    private javax.swing.JRadioButton UpperLeftRadioButton;
    private javax.swing.JRadioButton UpperRightRadioButton;
    private javax.swing.ButtonGroup VehLocationButtonGroup1;
    private javax.swing.JTextField WidthField;
    private javax.swing.JLabel WidthLabel;
    // End of variables declaration//GEN-END:variables

}
