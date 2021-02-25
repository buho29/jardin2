export default {
    // para formatear Time
    methods: {
        formatTime(utc) {
            let s = Math.floor(utc % 60);
            let m = Math.floor((utc / 60) % 60);
            let h = Math.floor((utc / (60 * 60)) % 24);
            let d = Math.floor(utc / (60 * 60 * 24));

            s = s < 10 ? "0" + s : "" + s;
            m = m < 10 ? "0" + m : "" + m;
            h = h < 10 ? "0" + h : "" + h;

            return h + ":" + m + ":" + s;
        },
        getTimeFromFormat(str) {
            let i = str.split(":");
            let r = parseInt(i[0]) * 3600;
            r += parseInt(i[1]) * 60;
            if (i.length > 2) r += parseInt(i[2]);
            return r;
        }
    },
}