(function() {
    randar.component('resource-dialog-area', {
        data: function() {
            return {
                resources: randar.resourcesWithDialogs
            };
        },

        template: `
            <div id="resource-dialog-area">
                <resource-dialog v-for="(resource, resourceId) in resources"
                 :key="resourceId"
                 :resource="resource">
                </resource-dialog>
            </div>
        `
    });
})();
