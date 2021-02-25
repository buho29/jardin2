
import {mapState} from 'vuex'
export default {
    computed: {
        //importamos datos
        ...mapState(['config']),
    },
    watch: {
        // los datos cambiaron 
        config: function (newValue, oldValue) {
            this.update();
        }
    },
    mounted() {
        this.update();
    },
}