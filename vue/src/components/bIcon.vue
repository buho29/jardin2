<template>
  <div :class="'icon-' + icon" style="text-shadow: 2px 2px 4px #999999">
    <span class="path1"></span><span class="path2"></span
    ><span class="path3"></span>
  </div>
</template>
<script>
export default {
  name: "b-icon",
  props: ["icon"],
};
</script>