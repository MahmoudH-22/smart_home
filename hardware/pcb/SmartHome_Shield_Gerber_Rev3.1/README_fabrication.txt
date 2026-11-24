SMART HOME SHIELD Rev 3.1 - fabrication notes
Single-sided Arduino Uno R3 shield, copper on the BOTTOM only. 68.58 x 53.34 mm (Uno outline), FR4 1.6 mm, 1 oz.
Files: B_Cu (copper), B_Mask (solder mask, bottom), F_Silkscreen (legend, component side),
Edge_Cuts (outline), PTH.drl (component holes). No mounting holes: the shield is held by the Uno headers.
Drills: 0.8 mm x 28, 0.9 mm x 2, 1.0 mm x 75 (total 105).
Tracks 0.8 mm (signals) / 1.0 mm (power), min clearance 0.7 mm, round pads 1.7-1.9 mm.
No wire links needed.
Keep the bottom copper away from the Uno USB-B jack, DC jack and ICSP headers (already respected);
still put insulating tape on top of the USB-B jack before stacking.
