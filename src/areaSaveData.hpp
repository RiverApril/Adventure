// Generated by genAreas.py

#ifndef areaSaveData_hpp
#define areaSaveData_hpp

struct AreaDataCastleGFKitchen{
    bool switchHit = false;
    bool doorClosed = true;
};
struct AreaDataCastleGFStorageRoom{
    bool chestOpen = false;
};
struct AreaDataCastleGFThrone{
    bool leftDoorLocked = true;
    bool topDoorLocked = true;
};


struct AreaSaveData {
    AreaDataCastleGFKitchen areaDataCastleGFKitchen;
    AreaDataCastleGFStorageRoom areaDataCastleGFStorageRoom;
    AreaDataCastleGFThrone areaDataCastleGFThrone;
};

#endif