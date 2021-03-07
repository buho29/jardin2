export const getZoneById = (state) => (id) => {
    return state.zones.find(zone => zone.id === id);
}

export const getZoneByAlarmId = (state, getters) => (id) => {
    let alarm = getters.getAlarmById(id);
    if(alarm)
        return getters.getZoneById(alarm.zoneId);
    else
        return undefined;
}

export const getZoneByName = (state) => (name) => {
    return state.zones.find(zone => zone.name === name);
}
export const getTapById = (state) => (id) => {
    return state.taps.find(tap => tap.id === id);
}
export const getAllAlarmsSize = (state) => (id) => {
    return state.alarms.filter(alarm => alarm.zoneId === id).length;
}
export const getAllAlarms = (state) => (id) => {
    return state.alarms.filter(alarm => alarm.zoneId === id);
}
export const getAlarmById = (state) => (id) => {
    return state.alarms.find(alarm => alarm.id === id);
}
export const getAlarmIndex = (state, getters) => (id) => {
    let alarm = getters.getAlarmById(id);
    let alarms = getters.getAllAlarms(alarm.zoneId);
    return alarms.findIndex(alarm => alarm.id === id);
}
export const getLastAlarm = (state, getters) => (id) => {
    let alarms = getters.getAllAlarms(id);
    if (alarms.length < 1) return undefined;
    let lastAlarm = alarms.reduce((a, c) => {
        return (a.time > c.time ? a : c);
    });
    return lastAlarm;
}