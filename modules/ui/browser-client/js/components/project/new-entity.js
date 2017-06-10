ui.component('new-entity', {
    data: function() {
        return {
            type : ui.params.get('type'),
            name : ''
        }
    },

    methods: {
        onClose: function() {
            ui.exit();
        },

        onSubmit: function() {
            if (this.type === 'bin') {
                ui.emit('project.folder.create', {
                    parentId : null,
                    name     : this.name
                });
            }

            ui.exit();
        }
    },

    template: `
        <div class="dialog">
            <menu-bar :onClose="onClose"></menu-bar>

            <div v-if="type === 'bin'">
                <input type="text" v-model="name">
                <input type="submit" value="*" @click="onSubmit">
            </div>
        </div>
    `
});
