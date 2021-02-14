// para formatear Time
var mixinFormat = {
    methods: {
        formatTime: function (utc) {


            let s = Math.floor(utc % 60);
            let m = Math.floor((utc / 60) % 60);
            let h = Math.floor((utc / (60 * 60)) % 24);
            let d = Math.floor(utc / (60 * 60 * 24));

            s = s < 10 ? "0" + s : "" + s;
            m = m < 10 ? "0" + m : "" + m;
            h = h < 10 ? "0" + h : "" + h;

            return h + ":" + m + ":" + s;
        },
        getTimeFromFormat: function (str) {
            let i = str.split(":");
            let r = parseInt(i[0]) * 3600;
            r += parseInt(i[1]) * 60;
            if (i.length > 2) r += parseInt(i[2]);
            return r;
        }
    }
}

var mixinNotify = {
    methods: {
        notify: function (msg) {
            //quasar notify
            this.$q.notify({
                color: 'primary',
                textColor: 'white',
                icon: 'icon-cloud-check',
                message: msg
            });
        },
        notifyW: function (msg) {
            this.$q.notify({
                color: 'accent',
                textColor: 'dark',
                icon: 'icon-alert-circle',
                message: msg
            })
        },
    }
}

//restingir acceso a pagina
var mixinRequiresAuth = {
    // importamos un mixin para notificaciones
    mixins: [mixinNotify],
    computed: {
        // importamos de vuex si esta logeado
        ...Vuex.mapState(['authenticate'])
    },
    //interceptando el acceso a la pagina
    beforeRouteEnter(to, from, next) {
        next(vm => {
            // si no esta logueado
            if (!vm.authenticate) {
                // ir a la pagina de login
                vm.$router.push("/login")
                vm.notifyW("Requiere login!");
            }
        })
    }
}