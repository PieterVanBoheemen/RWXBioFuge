$fn=40; // resolution of the arcs

pos1=18;
pos2=36;
pos3=54;
pos4=72;
distance=10.5;

scale([10,10,10]) { // cm to mm scaling

difference () { // substract tube holders from outer ring

difference () { // substract inner groove

	difference() { // substract cone from main disk

		translate([0,0,-0.4])
		cylinder (h = 4, r=10); // main disk

		translate([0,0,1]) // raise cone from bottom
		cylinder (h = 2.7, r1 = 7, r2 = 9.8, center = false); // cone

		translate([0,0,-0.6])
		cylinder (h = 1.6, r=0.3175); // axis

		translate([0,0,0.5]) // groove around axis
		cylinder (h = 0.5, r=1);

	}

	// groove under tube holders
	translate([0,0,0.5])
	difference() { // ring
		cylinder (h = 0.5, r = 7);
		cylinder (h = 0.5, r = 6.5);
	}

}

//tube holding cylinders
translate([-1*distance,0,-1]) rotate([0,35,0]) cylinder(h=8, r=0.6);
translate([distance,0,-1]) rotate([0,-35,0]) cylinder(h=8, r=0.6);

translate([0,-1*distance,-1]) rotate([0,35,90]) cylinder(h=8, r=0.6);
translate([0,distance,-1]) rotate([0,-35,90]) cylinder(h=8, r=0.6);

translate([-1 * distance * cos(pos1),-1 * distance * sin(pos1),-1]) rotate ([0,35,pos1]) cylinder(h=8, r=0.6);
translate([-1 * distance * cos(pos2),-1 * distance * sin(pos2),-1]) rotate ([0,35,pos2]) cylinder(h=8, r=0.6);
translate([-1 * distance * cos(pos3),-1 * distance * sin(pos3),-1]) rotate ([0,35,pos3]) cylinder(h=8, r=0.6);
translate([-1 * distance * cos(pos4),-1 * distance * sin(pos4),-1]) rotate ([0,35,pos4]) cylinder(h=8, r=0.6);

translate([-1 * distance * cos(pos1), 1 * distance * sin(pos1),-1]) rotate([0,35,-1*pos1]) cylinder(h=8, r=0.6);
translate([-1 * distance * cos(pos2), 1 * distance * sin(pos2),-1]) rotate([0,35,-1*pos2]) cylinder(h=8, r=0.6);
translate([-1 * distance * cos(pos3), 1 * distance * sin(pos3),-1]) rotate([0,35,-1*pos3]) cylinder(h=8, r=0.6);
translate([-1 * distance * cos(pos4), 1 * distance * sin(pos4),-1]) rotate([0,35,-1*pos4]) cylinder(h=8, r=0.6);

translate([distance * cos(pos1), -1 * distance * sin(pos1),-1]) rotate([0,-35,-1 * pos1]) cylinder(h=8, r=0.6);
translate([distance * cos(pos2), -1 * distance * sin(pos2),-1]) rotate([0,-35,-1 * pos2]) cylinder(h=8, r=0.6);
translate([distance * cos(pos3), -1 * distance * sin(pos3),-1]) rotate([0,-35,-1 * pos3]) cylinder(h=8, r=0.6);
translate([distance * cos(pos4), -1 * distance * sin(pos4),-1]) rotate([0,-35,-1 * pos4]) cylinder(h=8, r=0.6);

translate([distance * cos(pos1), distance * sin(pos1),-1]) rotate([0,-35,pos1]) cylinder(h=8, r=0.6);
translate([distance * cos(pos2), distance * sin(pos2),-1]) rotate([0,-35,pos2]) cylinder(h=8, r=0.6);
translate([distance * cos(pos3), distance * sin(pos3),-1]) rotate([0,-35,pos3]) cylinder(h=8, r=0.6);
translate([distance * cos(pos4), distance * sin(pos4),-1]) rotate([0,-35,pos4]) cylinder(h=8, r=0.6);
}

}