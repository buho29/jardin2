const routes = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { name: 'home', path: '', component: () => import('pages/Home.vue')},
      { name: 'charts', path: '/chart', component: () => import('pages/Charts.vue')},
      { name: 'zones', path: '/zones/:id', component: () => import('pages/Zones.vue')},
      { name: 'zone', path: '/zone/:id', component: () => import('pages/Zone.vue')},
      { name: 'taps', path: '/taps', component: () => import('pages/Taps.vue')},
      { name: 'login', path: '/login', component: () => import('pages/Login.vue')},
      { name: 'options', path: '/options', component: () => import('pages/Options.vue')},
    ]
  },

  // Always leave this as last one,
  // but you can also remove it
  {
    path: '*',
    component: () => import('pages/Error404.vue')
  }
]

export default routes
