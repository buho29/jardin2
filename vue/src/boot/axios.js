import axios from 'axios'

export default ({store , Vue}) => {
    Vue.prototype.$axios = axios;
    Vue.prototype.$window = window;
    store.$axios = axios;
}

