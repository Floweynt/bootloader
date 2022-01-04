#ifndef __STAGE2_REALMODE_REALMODE_H__
#define __STAGE2_REALMODE_REALMODE_H__
#include <cstdint>

struct memory_map
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t extend_bitfield;
};

struct far_ptr
{
    uint16_t segment;
    uint16_t offset;

    inline uint32_t to_ptr()
    {
        return (segment << 4) + offset;
    }
};

struct vbe_controller_info 
{
   uint32_t signature[4];
   uint16_t version;
   far_ptr oem_string_ptr;
   uint8_t capabilities[4];
   far_ptr video_mode_ptr;
   uint16_t total_memory_used;
} __attribute__((packed));

struct vbe_mode_info_structure
{
    uint16_t attributes;            // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a;               // deprecated
    uint8_t window_b;               // deprecated
    uint16_t granularity;           // deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;          // deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch;                 // number of bytes per horizontal line
    uint16_t width;                 // width in pixels
    uint16_t height;                // height in pixels
    uint8_t w_char;                 // unused...
    uint8_t y_char;                 // ...
    uint8_t planes;
    uint8_t bpp;                    // bits per pixel in this mode
    uint8_t banks;                  // deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size;              // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
 
    uint32_t framebuffer;           // physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__ ((packed));

struct bootloader_packet
{
    uint32_t loaded_address; // DONE
    uint32_t loaded_size;
    uint32_t mmap_size;
    uint32_t mmap_ptr;

    uint8_t boot_device;

    bool found_valid_vbe;

    uint16_t vbe_version;
    uint16_t total_memory_used;
    uint16_t width;
    uint16_t  height;
    uint8_t bpp;

    uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;

    uint32_t framebuffer;
};

extern bootloader_packet packet;

memory_map* read_memory_map();
vbe_controller_info* read_controller_info();
vbe_mode_info_structure* read_mode_info(uint16_t);
void set_mode(vbe_mode_info_structure* mode_struct, uint16_t mode);
void panic(const char*);

#endif