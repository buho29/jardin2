<script>
export default {
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
</script>