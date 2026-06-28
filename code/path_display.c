/*
 * path_display.c
 *
 *  Created on: 2026年6月23日
 *      Author: HP
 */

#include "zf_common_headfile.h"



#define PATH_DISPLAY_EPSILON (1e-6)

static int16 g_disp_x = 0;
static int16 g_disp_y = 0;
static int16 g_disp_w = 240;
static int16 g_disp_h = 180;

//仅在显示区域画点，超出就直接返回
static void path_display_point_safe(int x, int y, uint16 color)
{
    if(x < g_disp_x || x > g_disp_x + g_disp_w - 1) return;
    if(y < g_disp_y || y > g_disp_y + g_disp_h - 1) return;
    ips200_draw_point((uint16)x, (uint16)y, color);
}

//画圆
static void path_display_circle(uint16 xc, uint16 yc, uint16 radius, uint16 color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    int cx = (int)xc;
    int cy = (int)yc;

    while(y >= x)
    {
        path_display_point_safe(cx + x, cy + y, color);
        path_display_point_safe(cx - x, cy + y, color);
        path_display_point_safe(cx + x, cy - y, color);
        path_display_point_safe(cx - x, cy - y, color);
        path_display_point_safe(cx + y, cy + x, color);
        path_display_point_safe(cx - y, cy + x, color);
        path_display_point_safe(cx + y, cy - x, color);
        path_display_point_safe(cx - y, cy - x, color);

        if(d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
//将整数限制在指定范围内，放置屏幕坐标超出显示区域
static int16 path_display_clamp_i16(int value, int16 min_value, int16 max_value)
{
    if(value < min_value) return min_value;
    if(value > max_value) return max_value;
    return (int16)value;
}
//将路径坐标系转换为屏幕坐标系
static void path_display_to_screen(const path_display_transform_t *t,
                                   double x,
                                   double y,
                                   uint16 *sx,
                                   uint16 *sy)
{
    int px = (int)((x - t->min_x) * t->scale + t->x_offset + 0.5);
    int py = (int)((double)(g_disp_h - 1) - ((y - t->min_y) * t->scale + t->y_offset) + 0.5);

    px += g_disp_x;
    py += g_disp_y;

    *sx = (uint16)path_display_clamp_i16(px, g_disp_x, g_disp_x + g_disp_w - 1);
    *sy = (uint16)path_display_clamp_i16(py, g_disp_y, g_disp_y + g_disp_h - 1);
}
//计算路径坐标系到屏幕坐标系的转换参数
static uint8 path_display_calc_transform(const path_t *p,
                                         uint8 show_current,
                                         path_display_transform_t *t)
{
    int16 num;
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    double width;
    double height;
    double scale_x;
    double scale_y;

    if(p == NULL || p->x == NULL || p->y == NULL || p->num == NULL)
        return 0;

    num = *(p->num);
    if(num <= 0)
        return 0;

    min_x = p->x[0];
    max_x = p->x[0];
    min_y = p->y[0];
    max_y = p->y[0];

    for(int16 i = 1; i < num; i++)
    {
        min_x = fmin(min_x, p->x[i]);
        max_x = fmax(max_x, p->x[i]);
        min_y = fmin(min_y, p->y[i]);
        max_y = fmax(max_y, p->y[i]);
    }

    if(show_current && p->cur_x != NULL && p->cur_y != NULL)
    {
        min_x = fmin(min_x, *(p->cur_x));
        max_x = fmax(max_x, *(p->cur_x));
        min_y = fmin(min_y, *(p->cur_y));
        max_y = fmax(max_y, *(p->cur_y));
    }

    width = max_x - min_x;
    height = max_y - min_y;

    if(width < PATH_DISPLAY_EPSILON)
    {
        min_x -= 1.0;
        max_x += 1.0;
        width = max_x - min_x;
    }
    if(height < PATH_DISPLAY_EPSILON)
    {
        min_y -= 1.0;
        max_y += 1.0;
        height = max_y - min_y;
    }

    scale_x = (double)(g_disp_w - 1) / width;
    scale_y = (double)(g_disp_h - 1) / height;

    t->min_x = min_x;
    t->min_y = min_y;
    t->scale = fmin(scale_x, scale_y);
    t->x_offset = ((double)(g_disp_w - 1) - width * t->scale) / 2.0;
    t->y_offset = ((double)(g_disp_h - 1) - height * t->scale) / 2.0;

    return 1;
}
//初始化显示区域参数
void path_display_init(int16 start_x, int16 start_y, int16 width, int16 height)
{
    g_disp_x = start_x;
    g_disp_y = start_y;
    g_disp_w = width;
    g_disp_h = height;

    if(g_disp_w < 4) g_disp_w = 4;
    if(g_disp_h < 4) g_disp_h = 4;
}
//渲染路径到屏幕，包括清屏-坐标转换-画边框-画路径-画点-画目标点-画当前位置-显示点数和目标索引
void path_display_render(const path_t *p, uint8 show_current)
{
    path_display_transform_t t;
    int16 num;
    int16 footer_y = g_disp_y + g_disp_h + 4;

    ips200_clear();

    if(!path_display_calc_transform(p, show_current, &t))
    {
        ips200_show_string(0, 0, "No Path");
        return;
    }

    num = *(p->num);

    ips200_draw_line(g_disp_x, g_disp_y, g_disp_x + g_disp_w - 1, g_disp_y, RGB565_GRAY);
    ips200_draw_line(g_disp_x, g_disp_y, g_disp_x, g_disp_y + g_disp_h - 1, RGB565_GRAY);
    ips200_draw_line(g_disp_x + g_disp_w - 1, g_disp_y,
                     g_disp_x + g_disp_w - 1, g_disp_y + g_disp_h - 1, RGB565_GRAY);
    ips200_draw_line(g_disp_x, g_disp_y + g_disp_h - 1,
                     g_disp_x + g_disp_w - 1, g_disp_y + g_disp_h - 1, RGB565_GRAY);

    for(int16 i = 0; i < num - 1; i++)
    {
        uint16 x0;
        uint16 y0;
        uint16 x1;
        uint16 y1;

        path_display_to_screen(&t, p->x[i], p->y[i], &x0, &y0);
        path_display_to_screen(&t, p->x[i + 1], p->y[i + 1], &x1, &y1);
        ips200_draw_line(x0, y0, x1, y1, RGB565_CYAN);
    }

    for(int16 i = 0; i < num; i++)
    {
        uint16 x;
        uint16 y;

        path_display_to_screen(&t, p->x[i], p->y[i], &x, &y);
        path_display_circle(x, y, 2, RGB565_GREEN);
    }

    if(p->target_idx != NULL && *(p->target_idx) >= 0 && *(p->target_idx) < num)
    {
        uint16 x;
        uint16 y;

        path_display_to_screen(&t, p->x[*(p->target_idx)], p->y[*(p->target_idx)], &x, &y);
        path_display_circle(x, y, 4, RGB565_YELLOW);
    }

    if(show_current && p->cur_x != NULL && p->cur_y != NULL)
    {
        uint16 x;
        uint16 y;
        uint16 x_l;
        uint16 x_r;
        uint16 y_u;
        uint16 y_d;

        path_display_to_screen(&t, *(p->cur_x), *(p->cur_y), &x, &y);
        path_display_circle(x, y, 5, RGB565_RED);
        x_l = (uint16)path_display_clamp_i16((int)x - 4, g_disp_x, g_disp_x + g_disp_w - 1);
        x_r = (uint16)path_display_clamp_i16((int)x + 4, g_disp_x, g_disp_x + g_disp_w - 1);
        y_u = (uint16)path_display_clamp_i16((int)y - 4, g_disp_y, g_disp_y + g_disp_h - 1);
        y_d = (uint16)path_display_clamp_i16((int)y + 4, g_disp_y, g_disp_y + g_disp_h - 1);
        ips200_draw_line(x_l, y, x_r, y, RGB565_RED);
        ips200_draw_line(x, y_u, x, y_d, RGB565_RED);
    }

    ips200_show_string(0, footer_y, "N");
    ips200_show_int(16, footer_y, num, 3);
    if(p->target_idx != NULL)
    {
        ips200_show_string(64, footer_y, "T");
        ips200_show_int(80, footer_y, *(p->target_idx), 3);
    }
}



