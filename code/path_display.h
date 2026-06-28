/*
 * path_display.h
 *
 *  Created on: 2026年6月23日
 *      Author: HP
 */

#ifndef CODE_PATH_DISPLAY_H_
#define CODE_PATH_DISPLAY_H_



typedef struct {
    double min_x;
    double min_y;
    double scale;
    double x_offset;
    double y_offset;
}path_display_transform_t;

static void path_display_point_safe(int x, int y, uint16 color);

void path_display_init(int16 start_x, int16 start_y, int16 width, int16 height);
void path_display_render(const path_t *p, uint8 show_current);



#endif /* CODE_PATH_DISPLAY_H_ */
