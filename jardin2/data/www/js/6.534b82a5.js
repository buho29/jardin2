(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([[6],{"1e5b":function(t,e,a){},"2da0":function(t,e,a){"use strict";a("1e5b")},"713b":function(t,e,a){"use strict";a.r(e);var n=function(){var t=this,e=t.$createElement,a=t._self._c||e;return a("q-layout",{attrs:{view:"hHh LpR ffr"}},[a("q-header",{attrs:{elevated:""}},[a("q-toolbar",[a("q-btn",{attrs:{flat:"",dense:"",round:"","aria-label":"Menu",icon:"icon-menu"},on:{click:function(e){t.leftDrawerOpen=!t.leftDrawerOpen}}}),a("q-toolbar-title",[t._v(t._s(t.$t("titles."+t.$route.name)))]),t.authenticate?a("q-btn",{attrs:{flat:"",round:""},on:{click:function(e){return t.logout()}}},[t._v("logout")]):a("q-btn",{attrs:{flat:"",round:"",to:"/login"}},[t._v("login")])],1)],1),a("q-drawer",{attrs:{"show-if-above":"",bordered:"","content-class":"bg-grey-1"},model:{value:t.leftDrawerOpen,callback:function(e){t.leftDrawerOpen=e},expression:"leftDrawerOpen"}},[a("q-list",[a("q-item-label",{attrs:{header:""}},[t._v("Navegacion")]),t._l(t.menu,(function(e,n){return a("q-item",{key:n,attrs:{exact:"",to:e.path}},[a("q-item-section",{attrs:{avatar:""}},[a("q-icon",{attrs:{name:e.icon}})],1),a("q-item-section",[a("q-item-label",[t._v(t._s(t.$t("titles."+e.title)))])],1)],1)}))],2)],1),a("q-page-container",[a("transition",{attrs:{name:t.transitionName}},[a("router-view",{staticClass:"child-view"})],1)],1)],1)},o=[],i=a("2f62");const r=[{title:"home",icon:"icon-sun-cloud",path:"/"},{title:"charts",icon:"icon-stats-dots",path:"/chart"},{title:"zones",icon:"icon-watering-can",path:"/zones/0"},{title:"taps",icon:"icon-water-pump",path:"/taps"},{title:"options",icon:"icon-cog",path:"/options"}];var c={name:"MainLayout",data(){return{leftDrawerOpen:!1,transitionName:"slide-right",menu:r}},computed:{...Object(i["e"])(["authenticate"])},methods:{...Object(i["d"])(["logout"])},watch:{$route(t,e){this.transitionName="slide-left"===this.transitionName?"slide-right":"slide-left"}}},l=c,s=(a("2da0"),a("2877")),u=a("4d5a"),d=a("e359"),p=a("65c6"),m=a("9c40"),f=a("6ac5"),h=a("9404"),b=a("1c1c"),w=a("0170"),q=a("66e5"),v=a("4074"),g=a("0016"),Q=a("09e3"),_=a("eebe"),O=a.n(_),D=Object(s["a"])(l,n,o,!1,null,null,null);e["default"]=D.exports;O()(D,"components",{QLayout:u["a"],QHeader:d["a"],QToolbar:p["a"],QBtn:m["a"],QToolbarTitle:f["a"],QDrawer:h["a"],QList:b["a"],QItemLabel:w["a"],QItem:q["a"],QItemSection:v["a"],QIcon:g["a"],QPageContainer:Q["a"]})}}]);