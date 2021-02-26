<template>
  <div class="q-mt-xl" style="width: 100%">
    <q-btn :label="$t('send')" type="submit" color="primary" />
    <q-btn :label="$t('reset')" type="reset" color="primary" flat class="q-ml-sm"/>
  </div>
</template>
<script>
export default {
    name:'b-btn-send'
}
</script>