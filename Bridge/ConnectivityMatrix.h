#include <Arduino.h>

#ifndef CONNECTIVITYMATRIX_H
#define CONNECTIVITYMATRIX_H

// This is for display of connectivity status on a 12x8 led display
enum ConnectivityFrame {
    WifiDownLoraDown = 0,
    WifiUpLoraUp = 1,
    WifiDownLoraUp = 2,
    WifiUpLoraDown = 3,
    Unknown1 = 4,
    Unknown2 = 5,
    WifiUploading = 6,
    WifiDownloading = 7,
    WifiSuccess = 8,
    WifiFail = 9,
    LoraUploading = 10,
    LoraDownloading = 11,
    LoraSuccess = 12,
    LoraFail = 13,
    StatusBusy = 14,
	ClusterStatusError = 15,
	ClusterStatusOk = 16,
	ClusterOnboard = 17
};

const uint32_t ConnectivityMatrixAnimation[][4] = {
	{
		0x44854854,
		0x828e0002,
		0x8a10428a,
		66
	},
	{
		0x44854854,
		0x828e0003,
		0x8e000000,
		66
	},
	{
		0x44854854,
		0x828e0002,
		0x8e100280,
		66
	},
	{
		0x44854854,
		0x828e0003,
		0x8a00400a,
		66
	},
	{
		0x44854854,
		0x828e0003,
		0x8e28a38e,
		66
	},
	{
		0x44854854,
		0x828e0000,
		0x104000,
		66
	},
	{
		0x11015015,
		0xa400e0,
		0x1f004004,
		66
	},
	{
		0x11015015,
		0xa40040,
		0x1f00e004,
		66
	},
	{
		0x11015015,
		0xa10020,
		0x4028010,
		66
	},
	{
		0x11015015,
		0xa000a0,
		0x400a000,
		66
	},
	{
		0x10010010,
		0x1c400e0,
		0x1f004004,
		66
	},
	{
		0x10010010,
		0x1c40040,
		0x1f00e004,
		66
	},
	{
		0x10010010,
		0x1c10020,
		0x4028010,
		66
	},
	{
		0x10010010,
		0x1c000a0,
		0x400a000,
		66
	},
	{
		0x80100000,
		0x0,
		0x801,
		66
	},
	{
		0x20410809,
		0x600600,
		0x90108204,
		66
	},
	{
		0x200,
		0x40080102,
		0x20140080,
		66
	},
	{
		0x78,
		0xa48a4fe4,
		0x80780000,
		66
	}	
};

#endif