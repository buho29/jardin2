<template>
  <div
    class="q-ma-lg q-mx-auto text-center items-center bg-white"
    style="min-width: 300px; max-width: 600px; width: 90%"
  >
    <div class="bg-primary text-white shadow-3">
      <div class="text-h6 q-pa-sm">{{ title }}</div>
    </div>

    <div class="q-pa-md">
      <slot></slot>
    </div>
  </div>
</template>
<script>
export default {
  name: "b-container",
  props: ["title"],
};
</script>