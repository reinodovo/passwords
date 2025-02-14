include <../../ktane-3d-models/variables.scad>;
use <../../ktane-3d-models/bomb_base.scad>;

$fn = $preview ? 10 : 50;

tolerance = 0.3;

display_real_length = 75.66;
display_hidden_wall_thickness = 0.6;
display_length = display_real_length / 6 * 5;
display_width = 21.2;
display_position_x = 7.4 + display_length / 2;
display_position_y = 34.5 + display_width / 2;

button_base = 7.8;
button_height = 7.2; 
button_position_x = 13.43;
button_position_y = 27.4;
button_gap_x = 12.7;
button_gap_y = 35.56;

submit_length = 20.5;
submit_width = 8;
submit_position_x = 38.83;
submit_position_y = 10.89;

module top() {
    button_base_tol = button_base + tolerance;
    button_height_tol = button_height + tolerance;
    difference() {
        bomb_module_top(height_above_pcb = 7.8 + display_hidden_wall_thickness, module_type = 1);
        translate([display_position_x, display_position_y])
            cube([display_length + tolerance, display_width + tolerance, 20], center = true);
        translate([display_position_x - display_length / 2 + display_real_length / 2, display_position_y, - 10 - display_hidden_wall_thickness])
            cube([display_real_length + tolerance, display_width + tolerance, 20], center = true);
        translate([submit_position_x, submit_position_y])
            cube([submit_length + tolerance, submit_width + tolerance, 20], center = true);
        for (i = [0:4]) {
            for (j = [0:1]) {
                translate([button_position_x + i * button_gap_x, button_position_y + j * button_gap_y, -10]) rotate([0, 0, 180 * (1 - j)]) linear_extrude(20) polygon([[-button_base_tol / 2, -button_height_tol / 3], [button_base_tol / 2, -button_height_tol / 3], [0, 2 * button_height_tol / 3]]);
            }
        }
    }
}

module bottom() {
    bomb_module_bottom(height_above_pcb = 7.8 + display_hidden_wall_thickness);
}

top();
bottom();
