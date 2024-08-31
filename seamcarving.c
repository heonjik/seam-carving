#include "c_img.h"
#include "seamcarving.h"
#include <stdio.h>
#include <math.h>


void calc_energy(struct rgb_img *im, struct rgb_img **grad)
{
    create_img(grad, im->height, im->width);
    // i == height == y, j == width == x
    for(int j = 0; j < im->width; j++){
        for(int i = 0; i < im->height; i++){
            int delta_r_x; int delta_g_x; int delta_b_x; int delta_r_y; int delta_g_y; int delta_b_y;
            int r_x; int g_x; int b_x; int r_y; int g_y; int b_y;
            if(j == 0){
                delta_r_x = get_pixel(im, i, j+1, 0) - get_pixel(im, i, (im->width)-1, 0);
                delta_g_x = get_pixel(im, i, j+1, 1) - get_pixel(im, i, (im->width)-1, 1);
                delta_b_x = get_pixel(im, i, j+1, 2) - get_pixel(im, i, (im->width)-1, 2);
            }
            if(j == im->width - 1){
                delta_r_x = get_pixel(im, i, 0, 0) - get_pixel(im, i, j-1, 0);
                delta_g_x = get_pixel(im, i, 0, 1) - get_pixel(im, i, j-1, 1);
                delta_b_x = get_pixel(im, i, 0, 2) - get_pixel(im, i, j-1, 2);
            }
            else if(j != 0 && j != (im->width) - 1){
                delta_r_x = get_pixel(im, i, j+1, 0) - get_pixel(im, i, j-1, 0);
                delta_g_x = get_pixel(im, i, j+1, 1) - get_pixel(im, i, j-1, 1);
                delta_b_x = get_pixel(im, i, j+1, 2) - get_pixel(im, i, j-1, 2);
            }
            if(i == 0){
                delta_r_y = get_pixel(im, i+1, j, 0) - get_pixel(im, (im->height)-1, j, 0);
                delta_g_y = get_pixel(im, i+1, j, 1) - get_pixel(im, (im->height)-1, j, 1);
                delta_b_y = get_pixel(im, i+1, j, 2) - get_pixel(im, (im->height)-1, j, 2);
            }
            if(i == im->height - 1){
                delta_r_y = get_pixel(im, 0, j, 0) - get_pixel(im, i-1, j, 0);
                delta_g_y = get_pixel(im, 0, j, 1) - get_pixel(im, i-1, j, 1);
                delta_b_y = get_pixel(im, 0, j, 2) - get_pixel(im, i-1, j, 2);
            }
            else if(i != 0 && i != (im->height - 1)){
                delta_r_y = get_pixel(im, i+1, j, 0) - get_pixel(im, i-1, j, 0);
                delta_g_y = get_pixel(im, i+1, j, 1) - get_pixel(im, i-1, j, 1);
                delta_b_y = get_pixel(im, i+1, j, 2) - get_pixel(im, i-1, j, 2);
            }
            r_x = pow(delta_r_x, 2);
            g_x = pow(delta_g_x, 2); 
            b_x = pow(delta_b_x, 2);

            r_y = pow(delta_r_y, 2);
            g_y = pow(delta_g_y, 2);
            b_y = pow(delta_b_y, 2);

            uint8_t energy = (sqrt(r_x + g_x + b_x + r_y + g_y + b_y))/10;
            set_pixel(*grad, i, j, energy, energy, energy);
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
    // make an array
    int height = grad->height;
    int width = grad->width;
    *best_arr = (double *)malloc(sizeof(double) * height * width);

    // i == height == y, j == width == x
    for(int i = 0; i < height; i ++){
        if(i == 0){
            for(int k = 0; k < width; k++){
                (*best_arr)[k] = get_pixel(grad, i, k, 0);
            }
            continue;
        }
        for(int j = 0; j < width; j++){
            // 3 options: top-left (i-1, j-1), top-up (i-1, j), top_right (i-1, j+1)
            double min = (*best_arr)[(i-1)*width + j];
            if(j == 0){
                // 2 options: top-up (i-1, j), top-right (i-1, j+1)
                if(min > (*best_arr)[(i-1)*width + j+1]){
                    min = (*best_arr)[(i-1)*width + j+1];
                }
            }
            if(j > 0 && j < width-1){
                if(min > (*best_arr)[(i-1)*width + j-1]){
                    min = (*best_arr)[(i-1)*width + j-1];
                }
                if(min > (*best_arr)[(i-1)*width + j+1]){
                    min = (*best_arr)[(i-1)*width + j+1];
                }
            }
            if(j == width-1){
                // 2 options: top-left (i-1, j-1), top-up (i-1, j)
                if(min > (*best_arr)[(i-1)*width + j-1]){
                    min = (*best_arr)[(i-1)*width + j-1];
                }
            }
            // min + cur then store in *best_arr
            (*best_arr)[i*width + j] = min + get_pixel(grad, i, j, 0);
        }
    }
}


void recover_path(double *best, int height, int width, int **path)
{
    *path = (int *)malloc(sizeof(double) * height);
    // start from the bottom
    for(int i = height-1; i >= 0; i--){
        int index = 0;
        if(i == height-1){
            double cur_min = best[i * width];
            for(int k = 1; k < width; k++){
                if(best[i * width + k] < cur_min){
                    cur_min = best[i * width + k];
                    index = k;
                }
            }
        }
        else{
            index = (*path)[i+1];
            double cur_min = best[i*width + index];
            // 2 options: top-up, top-right
            if(index == 0){
                if(cur_min > best[i*width + index+1]){
                    cur_min = best[i*width + index+1];
                    index = index+1;
                }
            }
            // 2 options: top-left, top-up
            if(index == width-1){
                if(cur_min > best[i*width + index-1]){
                    cur_min = best[i*width + index-1];
                    index = index-1;
                }
            }
            // 3 options: top-left, top-up, top-right
            if(index != 0 && index != width-1){
                int ref_index = index - 1;
                cur_min = best[i*width + ref_index];
                int add = 0;
                for(int j = 1; j < 3; j++){
                    if(best[i*width + ref_index + j] < cur_min){
                        cur_min = best[i*width + ref_index + j];
                        if(j == 1){
                            add = 1;
                        }
                        if(j == 2){
                            add = 2;
                        }
                    }
                }
                // #define MIN(a, b) (((a<b)?a:b))
                // MIN(a, MIN(b, c))
                index = ref_index + add;
            }
        }
        (*path)[i] = index;
    }
}


void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)
{
    int height = src->height;
    int width = src->width;
    create_img(dest, height, width-1);

    for(int i = 0; i < height; i++){
        int r; int g; int b; int k = 0;
        for(int j = 0; j < width; j++){
            if(j == path[i]){
                j++;
                r = get_pixel(src, i, j, 0);
                g = get_pixel(src, i, j, 1);
                b = get_pixel(src, i, j, 2);
                set_pixel(*dest, i, k, r, g, b);
            }
            if(j != path[i]){
                r = get_pixel(src, i, j, 0);
                g = get_pixel(src, i, j, 1);
                b = get_pixel(src, i, j, 2);
                set_pixel(*dest, i, k, r, g, b);
            }
            k++;
        }
    }
}

int main()
{
    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best_arr;
    int *path;

    read_in_img(&im, "HJoceanSmall.bin");
    
    for(int i = 0; i < 150; i++){
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best_arr);
        recover_path(best_arr, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        write_img(cur_im, "new_HJoceanSmall.bin");

        destroy_image(im);
        destroy_image(grad);
        free(best_arr);
        free(path);
        im = cur_im;
    }
    destroy_image(im);
}