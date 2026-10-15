// =============================================================================
// Flapping Butterfly Clip - parametric CAD (OpenSCAD)
// Original design for this repo, inspired by the idea of a servo-driven pair of
// meshed gears flapping two lattice wings. NOT derived from any third-party STL.
//
// Choose what to export with  -D 'part="..."'  :
//   assembly | gear_drive | gear_idler | wing_left | wing_right | case_base | case_lid
//
// Axes: Z up. The two wing hinge axes are vertical, at x = -P/2 (drive) and x = +P/2 (idler).
// Wings at rest lie in the XZ plane (open); 90 degrees of gear rotation folds them back (-Y).
//
// !! Every number marked VERIFY is a typical value, not a measurement of YOUR parts.
//    Measure with calipers, edit the value, re-export. Print one test piece first.
// =============================================================================
$fn = 72;
part = "assembly";

// ---- Servo presets: [body_len, body_wid, body_h, flange_len, flange_t, flange_z, shaft_off, hole_spacing]
//      flange_z = height of the flange underside above the servo bottom; shaft_off = shaft distance
//      from the body centre along its length.  ALL VERIFY.
servo_model = "sg90";                  // "sg90" (SG92R/SG90 class) or "micro2g"
function sv() = servo_model == "sg90"
    ? [22.8, 12.4, 22.7, 32.5, 2.5, 15.5, 5.8, 27.8]
    : [16.5,  8.5, 17.0, 22.5, 1.5, 10.5, 4.0, 19.5];
sl = sv()[0]; sw = sv()[1]; sh = sv()[2];
fl = sv()[3]; ft = sv()[4]; fz = sv()[5];
s_off = sv()[6]; h_sp = sv()[7];

// ---- Gears (module 1, 20 deg pressure angle, 1:1 so both wings sweep the same angle) ----
m = 1.0;  N = 16;  P = m * N;          // P = centre distance = pitch diameter
gear_h = 5;                            // tooth face width
backlash = 0.2;                        // mm of play, raise if printed gears bind
horn_hub_d = 7.4;  horn_t = 4.0;       // VERIFY: servo horn hub diameter / thickness
horn_arm_len = 6.5;  horn_arm_w = 4.6; // trim the horn arm to this length with cutters
post_d = 9;  post_h = 7;               // wing socket on top of each gear
wing_t = 1.6;  slot_clear = 0.25;      // wing plate thickness, slot clearance

// ---- Wings ----
wing_span = 68;   wing_height = 62;    // size of one wing (mm)
rim_w = 2.2;  rib_w = 1.7;

// ---- Electronics (all VERIFY) ----
board_l = 23.0; board_w = 18.5; board_t = 1.6;   // ESP32-C3 + OLED board outline
oled_h  = 2.2;                                    // PCB top to top of OLED glass
oled_win = [11.5, 7.5];                           // window cut in the lid
oled_off = [0, 0];                                // window offset from board centre
batt = [30, 13, 6.5];                             // LiPo l x w x t (501230-class)
usb_w = 9.5; usb_h = 3.6;                         // USB-C opening

// ---- Case ----
wall = 1.8; floor_t = 2.0; lid_t = 2.0; gap = 1.0;
tol = 0.25;

// ----------------------------------------------------------------------------- derived
xc      = -P/2 + s_off;                   // servo body centre x (shaft sits at x = -P/2)
z_lid_u = floor_t + gap + fz + ft;        // lid underside; servo flange tops against it
z_lid_t = z_lid_u + lid_t;                // lid top face
servo_above_lid = sh - fz - ft;           // part of the servo that pokes through the lid
z_gear  = z_lid_t + servo_above_lid + 0.5;// gear bottom, just above servo top (horn inside gear)
ix0 = xc - fl/2 - 0.6;
ix1 = max(xc + fl/2, P/2 + 7) + 0.6;
bay_y0 = sw/2 + 2.0;                       // electronics bay starts beside the servo
iy0 = -sw/2 - 1.0;
iy1 = bay_y0 + board_w + 3.0;
ox0 = ix0 - wall;  ox1 = ix1 + wall;  oy0 = iy0 - wall;  oy1 = iy1 + wall;
board_cx = (ix0 + ix1)/2;
board_cy = bay_y0 + 1.0 + board_w/2;
pcb_top_z = z_lid_u - oled_h - 0.3;

// =============================================================================
// Involute spur gear (2D)
// =============================================================================
function inv_phi(t) = (t - atan(t) * PI/180);         // involute polar angle in rad (t in rad)
module tooth2d(rp, ra, rd, pa, half_deg) {
    rb = rp * cos(pa);
    tp = sqrt(pow(rp/rb,2) - 1);
    tt = sqrt(pow(ra/rb,2) - 1);
    steps = 8;
    a0 = half_deg + inv_phi(tp) * 180/PI;               // flank polar angle at the base circle
    pts_r = [ for (i=[0:steps]) let(t = tt*i/steps, r = rb*sqrt(1+t*t),
                a = a0 - inv_phi(t)*180/PI) [r*cos(a), r*sin(a)] ];
    pts_l = [ for (i=[steps:-1:0]) let(t = tt*i/steps, r = rb*sqrt(1+t*t),
                a = a0 - inv_phi(t)*180/PI) [r*cos(a), -r*sin(a)] ];
    root_r = [rd*cos(a0), rd*sin(a0)];
    root_l = [rd*cos(a0), -rd*sin(a0)];
    polygon(concat([root_r], pts_r, pts_l, [root_l]));
}
module gear2d(n = N, mod = m, pa = 20, bl = backlash) {
    rp = mod*n/2;  ra = rp + mod;  rd = rp - 1.25*mod;
    half = 90/n - (bl/(2*rp)) * 180/PI;                 // half tooth angle at pitch circle
    circle(r = rd + 0.01);
    for (k = [0:n-1]) rotate(k*360/n) tooth2d(rp, ra, rd, pa, half);
}

// =============================================================================
// Wing socket (post with a slot the wing foot slides into)
// =============================================================================
module wing_post() {
    difference() {
        cylinder(d = post_d, h = post_h);
        // slot runs along local X, so the wing plate lies in the XZ plane (open position)
        translate([-post_d, -(wing_t+slot_clear)/2, 1.5]) cube([2*post_d, wing_t+slot_clear, post_h]);
    }
}

module gear_drive() {
    difference() {
        union() {
            linear_extrude(gear_h) gear2d();
            translate([0,0,gear_h]) wing_post();
        }
        // horn pocket from below: hub + one arm channel (arm points along local +X)
        translate([0,0,-0.01]) cylinder(d = horn_hub_d + 0.3, h = horn_t);
        translate([0,-horn_arm_w/2,-0.01]) cube([horn_arm_len, horn_arm_w, horn_t - 0.8]);
        translate([0,0,-1]) cylinder(d = 2.4, h = gear_h + post_h + 2);   // horn screw access
    }
}
module gear_idler() {
    difference() {
        union() {
            // half-tooth phase so the teeth interleave with the drive gear
            rotate(180/N) linear_extrude(gear_h) gear2d();
            translate([0,0,gear_h]) wing_post();
        }
        translate([0,0,-1]) cylinder(d = 2.4, h = gear_h + post_h + 2);   // spins on the 2 mm lid pin
    }
}

// =============================================================================
// Wing (flat print orientation: plate in XY, thickness Z; hinge edge on the Y axis)
// =============================================================================
module wing_outline2d() {
    intersection() {
        union() {
            // forewing (large, upper) and hindwing (small, lower)
            translate([wing_span*0.40, wing_height*0.64]) rotate(16)
                scale([wing_span*0.44, wing_height*0.34]) circle(r = 1);
            translate([wing_span*0.30, wing_height*0.27]) rotate(-24)
                scale([wing_span*0.32, wing_height*0.24]) circle(r = 1);
        }
        translate([0, 0]) square([wing_span*2, wing_height*2]);   // keep u >= 0
    }
}
module wing2d() {
    difference() {
        // outline minus the open cells = rim + ribs
        wing_outline2d();
        difference() {
            offset(delta = -rim_w) wing_outline2d();
            // ribs: veins fanning out from near the hinge
            for (a = [-62 : 12 : 82])
                translate([0, wing_height*0.36]) rotate(a)
                    translate([0, -rib_w/2]) square([wing_span*1.3, rib_w]);
        }
    }
    // hinge spine so the wing is one rigid piece, and the foot that slots into the post
    translate([0, 0]) square([rib_w*1.6, wing_height*0.62]);
    translate([-post_d/2 + 1.2, -(post_h - 1.5) + 0.3]) square([post_d - 2.4, (post_h - 1.5) - 0.3 + 2]);
}
module wing_right() { linear_extrude(wing_t) wing2d(); }
module wing_left()  { mirror([1,0,0]) wing_right(); }

// =============================================================================
// Case: tray (base) + lid. The servo hangs from the lid by its flange; the lid carries
// the idler pin, the OLED window and the board retainers. Strap slots in the floor.
// =============================================================================
module case_base() {
    difference() {
        translate([ox0, oy0, 0]) cube([ox1-ox0, oy1-oy0, z_lid_u]);
        // main cavity
        translate([ix0, iy0, floor_t]) cube([ix1-ix0, iy1-iy0, z_lid_u]);
        // strap slots through the floor (zip tie / barrette bar), 16 mm apart
        for (sx = [-8, 8]) translate([board_cx + sx - 1.25, oy0 + 4, -1]) cube([2.5, (oy1-oy0) - 8, floor_t + 2]);
        // USB-C opening on the +X wall, centred on the board
        translate([ix1 - 1, board_cy - usb_w/2, pcb_top_z - board_t - usb_h/2 - 0.5])
            cube([wall + 2, usb_w, usb_h + 1]);
        // slide-switch opening on the +Y wall (SS12D00G ~ 8 x 4 mm)
        translate([board_cx - 4.2, iy1 - 1, floor_t + 2.5]) cube([8.4, wall + 2, 4.4]);
    }
    // battery cradle on the floor, under the board
    translate([board_cx - batt[0]/2 - 1, bay_y0 + 1.0 + (board_w - batt[1])/2 - 1, floor_t])
        difference() {
            cube([batt[0] + 2, batt[1] + 2, 2.5]);
            translate([1, 1, 0.01]) cube([batt[0], batt[1], 3]);
        }
    // touch-pad window: wall thinned to 1 mm on the -X end, TTP223 sits behind it (VERIFY module size)
    // (cut in the wall: nothing added here, see case_base cavity + thinning below)
}
module case_lid() {
    difference() {
        union() {
            translate([ox0, oy0, 0]) cube([ox1-ox0, oy1-oy0, lid_t]);
            // idler pin (printed in place, 2 mm)
            translate([P/2, 0, lid_t]) cylinder(d = 2.0, h = servo_above_lid + 0.5 + gear_h + 1);
            // servo flange screw bosses
            for (sx = [-1, 1]) translate([xc + sx*h_sp/2, 0, -2.5]) cylinder(d = 5, h = 2.5);
            // board retainer posts with lips
            for (bx = [-1, 1], by = [-1, 1])
                translate([board_cx + bx*(board_l/2 + 1.2), board_cy + by*(board_w/2 + 1.2), 0]) {
                    translate([0,0,-(z_lid_u - pcb_top_z + board_t + 1.0)])
                        cylinder(d = 2.4, h = z_lid_u - pcb_top_z + board_t + 1.0);
                    // lip under the PCB corner
                    translate([-bx*1.2 - 1.2, -by*1.2 - 1.2, -(z_lid_u - pcb_top_z + board_t + 1.0)])
                        cube([2.4, 2.4, 1.0]);
                }
        }
        // opening for the servo top (above the flange)
        translate([xc - (sl+tol)/2, -(sw+tol)/2, -1]) cube([sl + tol, sw + tol, lid_t + 2]);
        // flange screw pilot holes (2 mm self-tapping)
        for (sx = [-1, 1]) translate([xc + sx*h_sp/2, 0, -3]) cylinder(d = 1.8, h = lid_t + 4);
        // OLED window
        translate([board_cx + oled_off[0] - oled_win[0]/2, board_cy + oled_off[1] - oled_win[1]/2, -1])
            cube([oled_win[0], oled_win[1], lid_t + 2]);
    }
}

// =============================================================================
// Assembly preview (open position)
// =============================================================================
module assembly() {
    color("gray")  case_base();
    translate([0,0,z_lid_u]) color("silver") case_lid();
    color("goldenrod") translate([-P/2, 0, z_gear]) gear_drive();
    color("goldenrod") translate([ P/2, 0, z_gear]) gear_idler();
    // wings stand up in the post slots
    zbase = z_gear + gear_h + post_h;
    color("white") translate([-P/2, 0, zbase])
        rotate([90, 0, 0]) translate([0, 0, -wing_t/2]) wing_left();
    color("white") translate([ P/2, 0, zbase])
        rotate([90, 0, 0]) translate([0, 0, -wing_t/2]) wing_right();
    // servo ghost
    color("royalblue", 0.4) translate([xc - sl/2, -sw/2, floor_t + gap]) cube([sl, sw, sh]);
}

if (part == "assembly")        assembly();
else if (part == "gear_drive") gear_drive();
else if (part == "gear_idler") gear_idler();
else if (part == "wing_left")  wing_left();
else if (part == "wing_right") wing_right();
else if (part == "case_base")  case_base();
else if (part == "case_lid")   case_lid();
