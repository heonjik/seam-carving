# seam-carving
* **Seam-carving** is a content-aware image resizing technique where the image is reduced in size by one pixel of width (or height) at a time.
* A vertical seam in an image is a path of pixels connected from the top to the bottom with one pixel in each row; a horizontal seam is a path of pixels connected from the left to the right with one pixel in each column.
* Unlike standard content-agnostic resizing techniques (such as cropping and scaling), seam carving preserves the most interest features (aspect ratio, set of objects present, etc.) of the image.
* It is now a core feature in Adobe Photoshop and other computer graphics applications.
---
## 1. Dual-Gradient Energy Function
* The first step is to calculate the energy of a pixel, which is a measure of its perceptual importance – the higher the energy, the less likely that the pixel will be included as part of a seam.
* The energy of pixel $`\sqrt{\Delta_{x}^2(y,x)+\Delta_{y}^2(y,x)}`$.
  * $`\Delta_{x}^2(y,x)=R_x(y,x)^2+G_x(y,x)^2+B_x(y,x)^2`$.
  * $`\Delta_{y}^2(y,x)=R_y(y,x)^2+G_y(y,x)^2+B_y(y,x)^2`$.
* The corresponding function is `void calc_energy(struct rgb_img *im, struct rgb_img **grad);`.
## 2. Cost Array
* Define the function which allocates and computes the dunamic array `*best_arr`.
* `(*best_arr)[i*width+j]` contains the minimum cost of a seam from the top of grad to the point (i,j).
* The corresponding function is `dynamic_seam(struct rgb_img *grad, double **best_arr);`.

## 3. Recover the seam
* Write a function that allocates a path through the minimum seam as defined by the array best.
* The corresponding function is `void recover_path(double *best, int height, int width, int **path);`.

## 4. Write a function that removes the seam
* Write the function that creates the destination image, and writes to it the source image, with the seam removed.
* The corresponding function is `void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);`.
