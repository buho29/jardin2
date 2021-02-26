import axios from 'axios'

export default ({store , Vue}) => {
    Vue.prototype.$axios = axios;
    store.$axios = axios;
}

