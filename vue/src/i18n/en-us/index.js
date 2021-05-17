export default {
  titles:{
    home:'Home',
    charts:'Charts',
    zones:'Zones',
    zone:'Zone',
    taps:'Taps',
    login:'Login',
    options:'Options'
  },
  config:{
    general: 'General',
    browser:'Browser',
    history:'History',
    helpRouting:
      `Allows the server to redirect you to a page<br/> 
      for example when watering a zone`,
    time: 'Time',
    nameCity : 'City name',
    user:'User',
    helpAdmin:'Change login',
    files: 'Files',
    up: 'Upload file',
    info: 'Info',
    resetEsp: 'Reset esp32',
    weather: 'Weather',
    system: "System",
    formatDate:'YYYY-MM-DD HH:mm:ss',
  },
  weather: {
    name:'Weather',
    win: 'Wind',
    clouds: 'Cloudiness',
    precipitation: 'Precipitation',
    sun : 'Sun'
  },
  zones: {
    pause: 'Pause',
    water: 'Water',
    resume: 'Resume',
    todayWatering: 'Watering today',
    todayNotWatering: 'No watering today',
  },
  zone:{
    modes: 'Watering Modes',
    alarms: 'Alarms',
    alarm: 'Alarm',
    time: 'Time',
    duration: 'Duration (min)',
    durationErrors: [
      'It s wrong...',
      'Use from 1 to 180min',
    ],
    tap: 'Tap',
    tapErrors: 'Select a tap',
    lastAlarm: 'Last Alarm'
  },
  app:{
    disconnected: 'Disconnected!',
    connected: 'Connected!',
    error: 'an error has occurred!'
  },
  server:{
    0: 'Welcome {0}',
    1: 'Invalid username or password',
    2: 'Login required',
    3: 'not found',
    4: 'Error creating busy by {0}',
    5: ' desired: ',
    6: '{0} open',
    7: '{0} closed',
    8: '{0} paused',
    9: 'Correctly deleted {0}',
    10: 'Edited successfully',
    11: '{0}successfully created',
    12: 'Error opening {0}',
    13: 'Error closing {0}',
    14: 'Error pausing {0}',
    15: 'Error deleting {0}',
    16: 'Error editing {0}',
    17: 'Error creating {0}',
    18: 'I am watering',
    
    19: 'Deleted successfully {0}',
    20: 'Uploaded successfully {0}',
    21: 'Error deleting {0}',
    22: 'Error uploading {0}',
    23: 'Download error {0}', 
  },
  actions: {
    0: '. . {0} ',
    1: '{0} manual',
    2: 'Zone {0} alarm',
    3: 'Zone {0} manual',
    4: '. . Zone {0} ',
    5: 'System started {0}',
  },
  history: {
    system: 'System',
    paused: 'Paused',
    open: '{0} open',
    close: '{0} closed',
    pause: '{0} paused',
    unpause: '{0} resumed'
  }, 
  date: {
    days: 'Sunday_Monday_Tuesday_Wednesday_Thursday_Friday_Saturday'.split('_'),
    daysShort: 'Sun_Mon_Tue_Wed_Thu_Fri_Sat'.split('_'),
    months: 'January_February_March_April_May_June_July_August_September_October_November_December'.split('_'),
    monthsShort: 'Jan_Feb_Mar_Apr_May_Jun_Jul_Aug_Sep_Oct_Nov_Dec'.split('_'),
    firstDayOfWeek: 0, // 0-6, 0 - Sunday, 1 Monday, ...
    format24h: false,
    pluralDay: 'days'
  },
  modes: {
    0: 'Deactivate 24h',
    1: 'Meteorology',
    2: '@:sensors',
    3: 'Intervals date',
    4: '@:date.days.1',
    5: '@:date.days.2',
    6: '@:date.days.3',
    7: '@:date.days.4',
    8: '@:date.days.5',
    9: '@:date.days.6',
    10: '@:date.days.0',
  },
  localeMoment: 'en',
  accept: 'Accept',
  send: 'Send',
  reset: 'Reset',
  name: 'Name',
  pass: 'Password',
  temp: 'Temperature',
  hum: 'Humidity',
  press: 'Pressure',
  sensors:'Sensors',
  new: 'New',
  delete: 'Delete',
  edit: 'Edit',
  cancel: 'Cancel',
  save: 'Save',
  failed: 'las cagao!',
  success: 'Oh Sii baby!',
}
