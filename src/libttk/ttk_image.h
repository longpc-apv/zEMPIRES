
#ifndef TTK_IMAGE_H
#define TTK_IMAGE_H

typedef struct {
    uint64_t width;
    uint64_t height;
    uint32_t bpp;
    void* data;
} TtkImage;

#endif // TTK_IMAGE_H
