<template>
  <div class="row">
    <div class="col text-bold text-capitalize text-left">{{ title }}</div>
    <div class="col text-right"><slot></slot></div>
  </div>
</template>
<script>
export default {
  name: "b-param",
  props: ["title"],
};
</script>