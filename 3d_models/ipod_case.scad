/*
ESP32-S3 iPod Case - OpenSCAD Model
Generate STL files for 3D printing
*/

// =====================================================
// PARAMETERS - Adjust these for your specific needs
// =====================================================

// Case dimensions
case_length = 110;
case_width = 65;
case_height = 18;
wall_thickness = 2;

// Display dimensions (MSP2806)
display_length = 52;
display_width = 39;
display_recess = 1;
display_offset_y = 15; // Distance from top

// ESP32-S3 dimensions
esp32_length = 55;
esp32_width = 25.5;
esp32_height = 15;

// Rotary encoder
encoder_diameter = 20;
encoder_hole = 7;
encoder_offset_y = 25; // Distance from display

// Button positions (relative to encoder center)
button_diameter = 4;
button_spacing = 15; // Distance from encoder center

// USB-C cutout
usb_width = 10;
usb_height = 4.5;

// =====================================================
// MAIN ASSEMBLY
// =====================================================

// Uncomment the part you want to generate:
top_shell();
//bottom_shell();

// =====================================================
// TOP SHELL (Front with display and buttons)
// =====================================================

module top_shell() {
    difference() {
        // Main shell body
        shell_body();
        
        // Display window
        translate([case_width/2 - display_width/2, display_offset_y, case_height - display_recess]) {
            cube([display_width, display_length, display_recess + 1]);
        }
        
        // Rotary encoder hole
        translate([case_width/2, display_offset_y + display_length + encoder_offset_y, -1]) {
            cylinder(d=encoder_hole, h=case_height + 2);
        }
        
        // Button holes
        button_positions();
        
        // Snap-fit receptacles
        snap_fit_holes();
    }
    
    // Display mounting posts
    display_mounting_posts();
    
    // Encoder mounting ring
    encoder_mounting_ring();
}

// =====================================================
// BOTTOM SHELL (Back with component mounts)
// =====================================================

module bottom_shell() {
    difference() {
        // Main shell body
        shell_body();
        
        // USB-C cutout
        translate([case_width/2 - usb_width/2, -1, case_height - usb_height - 2]) {
            cube([usb_width, wall_thickness + 2, usb_height]);
        }
        
        // Ventilation holes
        ventilation_holes();
        
        // Component cavity
        translate([wall_thickness, wall_thickness, wall_thickness]) {
            cube([case_width - 2*wall_thickness, case_length - 2*wall_thickness, case_height]);
        }
    }
    
    // ESP32-S3 mounting channel
    esp32_mount();
    
    // Snap-fit tabs
    snap_fit_tabs();
    
    // Alignment posts
    alignment_posts();
}

// =====================================================
// BASIC SHELL SHAPE
// =====================================================

module shell_body() {
    // Rounded rectangle shell
    hull() {
        translate([5, 5, 0]) cylinder(r=5, h=case_height);
        translate([case_width-5, 5, 0]) cylinder(r=5, h=case_height);
        translate([5, case_length-5, 0]) cylinder(r=5, h=case_height);
        translate([case_width-5, case_length-5, 0]) cylinder(r=5, h=case_height);
    }
}

// =====================================================
// DISPLAY MOUNTING
// =====================================================

module display_mounting_posts() {
    // 4 mounting posts for M2.5 screws
    post_positions = [
        [case_width/2 - 25, display_offset_y + 5],
        [case_width/2 + 25, display_offset_y + 5],
        [case_width/2 - 25, display_offset_y + display_length - 5],
        [case_width/2 + 25, display_offset_y + display_length - 5]
    ];
    
    for (pos = post_positions) {
        translate([pos[0], pos[1], wall_thickness]) {
            difference() {
                cylinder(d=4.5, h=4);
                cylinder(d=2, h=5); // M2.5 hole
            }
        }
    }
}

// =====================================================
// ROTARY ENCODER MOUNTING
// =====================================================

module encoder_mounting_ring() {
    translate([case_width/2, display_offset_y + display_length + encoder_offset_y, wall_thickness]) {
        difference() {
            cylinder(d=encoder_diameter, h=1);
            cylinder(d=encoder_hole, h=2);
        }
    }
}

// =====================================================
// BUTTON POSITIONS
// =====================================================

module button_positions() {
    encoder_center_x = case_width/2;
    encoder_center_y = display_offset_y + display_length + encoder_offset_y;
    
    // Menu button (top-left)
    translate([encoder_center_x - button_spacing, encoder_center_y - button_spacing, -1]) {
        cylinder(d=button_diameter, h=case_height + 2);
    }
    
    // Play button (top-right)
    translate([encoder_center_x + button_spacing, encoder_center_y - button_spacing, -1]) {
        cylinder(d=button_diameter, h=case_height + 2);
    }
    
    // Previous button (left)
    translate([encoder_center_x - button_spacing*1.3, encoder_center_y, -1]) {
        cylinder(d=button_diameter, h=case_height + 2);
    }
    
    // Next button (right)
    translate([encoder_center_x + button_spacing*1.3, encoder_center_y, -1]) {
        cylinder(d=button_diameter, h=case_height + 2);
    }
}

// =====================================================
// ESP32-S3 MOUNTING
// =====================================================

module esp32_mount() {
    // Friction fit channel for ESP32-S3
    translate([case_width/2 - esp32_width/2 - 1, case_length - esp32_length - 10, wall_thickness]) {
        difference() {
            cube([esp32_width + 2, esp32_length + 2, esp32_height/2]);
            translate([1, 1, -1]) {
                cube([esp32_width, esp32_length, esp32_height/2 + 2]);
            }
        }
    }
}

// =====================================================
// SNAP-FIT MECHANISM
// =====================================================

module snap_fit_tabs() {
    // 4 snap-fit tabs around perimeter
    tab_positions = [
        [case_width/2, 5, 0],
        [case_width/2, case_length-5, 0],
        [5, case_length/2, 90],
        [case_width-5, case_length/2, 90]
    ];
    
    for (pos = tab_positions) {
        translate([pos[0], pos[1], case_height/2]) {
            rotate([0, 0, pos[2]]) {
                snap_fit_tab();
            }
        }
    }
}

module snap_fit_tab() {
    translate([0, 0, 0]) {
        cube([8, 1, 4], center=true);
        translate([0, 1.5, 0]) {
            cube([6, 1, 3], center=true);
        }
    }
}

module snap_fit_holes() {
    // Corresponding holes for snap-fit tabs
    tab_positions = [
        [case_width/2, 5, 0],
        [case_width/2, case_length-5, 0],
        [5, case_length/2, 90],
        [case_width-5, case_length/2, 90]
    ];
    
    for (pos = tab_positions) {
        translate([pos[0], pos[1], case_height/2 - 2]) {
            rotate([0, 0, pos[2]]) {
                cube([10, 3, 6], center=true);
            }
        }
    }
}

// =====================================================
// ALIGNMENT POSTS
// =====================================================

module alignment_posts() {
    // 2 alignment posts to prevent misalignment
    translate([15, 15, case_height - 5]) {
        cylinder(d=3, h=5);
    }
    translate([case_width-15, case_length-15, case_height - 5]) {
        cylinder(d=3, h=5);
    }
}

// =====================================================
// VENTILATION
// =====================================================

module ventilation_holes() {
    // 4 ventilation holes in back panel
    hole_positions = [
        [15, case_length - 15],
        [case_width - 15, case_length - 15],
        [15, case_length - 35],
        [case_width - 15, case_length - 35]
    ];
    
    for (pos = hole_positions) {
        translate([pos[0], pos[1], -1]) {
            cylinder(d=3, h=wall_thickness + 2);
        }
    }
}

// =====================================================
// UTILITY FUNCTIONS
// =====================================================

// Rounded cube
module rounded_cube(size, radius) {
    hull() {
        translate([radius, radius, 0]) 
            cylinder(r=radius, h=size[2]);
        translate([size[0]-radius, radius, 0]) 
            cylinder(r=radius, h=size[2]);
        translate([radius, size[1]-radius, 0]) 
            cylinder(r=radius, h=size[2]);
        translate([size[0]-radius, size[1]-radius, 0]) 
            cylinder(r=radius, h=size[2]);
    }
}