// SL 2020-12-23 @sylefeb
// https://github.com/sylefeb/Silice
// MIT license, see LICENSE_MIT in Silice repo root
// -------------------------

// pre-compilation script, embeds compiled code within spiflash image
$$sdcard_image_pad_size = 0
$$dofile('SOCs/pre_include_compiled.lua')

$$error('=======> Done! the spiflash image file is *** data.img ***. Please ignore the subsequent error messages.')
