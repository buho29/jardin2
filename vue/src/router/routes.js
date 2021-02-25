const routes = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { name: 'home', path: '', component: () => import('pages/Home.vue'), meta: { title: 'Jardin' } },
      { name: 'chart', path: '/chart', component: () => import('pages/Charts.vue'), meta: { title: 'Historial' } },
      { name: 'zones', path: '/zones/:id', component: () => import('pages/Zones.vue'), meta: { title: 'Zonas' } },
      { name: 'zone', path: '/zone/:id', component: () => import('pages/Zone.vue'), meta: { title: 'Zona' } },
      { name: 'taps', path: '/taps', component: () => import('pages/Taps.vue'), meta: { title: 'Grifos' } },
      { name: 'login', path: '/login', component: () => import('pages/Login.vue'), meta: { title: 'Login' } },
      { name: 'opciones', path: '/options', component: () => import('pages/Options.vue'), meta: { title: 'Opciones' } },
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
