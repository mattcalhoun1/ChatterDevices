#ifndef REMOTECONFIG_H
#define REMOTECONFIG_H

enum RemoteConfigType {
    RemoteConfigBattery = 'B',
    RemoteConfigPath = 'P',
    RemoteConfigMeshCacheClear = 'M',
    RemoteConfigMeshGraphClear = 'G',
    RemoteConfigPingTableClear = 'T',
    RemoteConfigEnableLearn = 'E',
    RemoteConfigDisableLearn = 'D',
    RemoteConfigMessagesClear = 'C'
};

#endif