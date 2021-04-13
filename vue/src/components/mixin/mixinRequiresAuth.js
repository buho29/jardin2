import mixinNotify from "./mixinNotify";
import {mapState} from 'vuex'

export default{
      // importamos un mixin para notificaciones
  mixins: [mixinNotify],
  computed: {
      // importamos de vuex si esta logeado
      ...mapState(['authenticate'])
  },
  //interceptando el acceso a la pagina
  beforeRouteEnter(to, from, next) {
      next(vm => {
          // si no esta logueado
          if (!vm.authenticate) {
              // ir a la pagina de login
              vm.$router.push("/login")
          }
      })
  }
}