<template>
  <div>
    <q-btn
      v-if="checked"
      @click="onClick"
      :disable="disable"
      class="bg-accent text-dark q-ma-sm"
      :style="styleW"
    >
      {{ labelOff }}
    </q-btn>
    <q-btn
      v-else
      @click="onClick"
      :disable="disable"
      class="bg-primary text-white q-ma-sm"
      :style="styleW"
    >
      {{ labelOn }}
    </q-btn>
  </div>
</template>

<script>

export default {
  name: "b-toggle",
  props: {
    labelOn: String,
    labelOff: String,
    checked: { type: Boolean, default: false },
    disable: { type: Boolean, default: false },
    width: { type: String, default: "120px" },
  },
  model: {
    prop: "checked",
    event: "change"
  },
  computed: {
    styleW() {
      return `width:${this.width}`;
    }
  },
  methods: {
    onClick() {
      this.$emit("change", !this.checked);
    }
  }
};
</script>
