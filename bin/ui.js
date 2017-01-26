var randar = {
    /**
     * Handles an engine log message.
     */
    onEngineLog: function(message) {
        var div = document.createElement('div');
        div.innerHTML = '[' + message.level + '] ' + message.contents;

        var logElement = getElement('#engine-log');
        logElement.insertBefore(div, logElement.firstChild);
    },

    /**
     * Retrieves all resources from the engine and shows them on the interface.
     */
    updateResources: function() {
        var data = JSON.parse(window.getResources());

        for (type in data) {
            if (isString(data[type])) {
                randar.resources[type] = data[type];
            }

            else {
                for (item in data[type]) {
                    randar.resources[type][item] = data[type][item];
                }
            }
        }

        // Update project name.
        var projectInfo = getElement('#main .randar');
        if (randar.resources.name) {
            projectInfo.innerHTML = randar.resources.name;
        } else {
            projectInfo.innerHTML = 'Untitled Project';
        }

        // Update list elements.
        var texturesList = getElement('#objects .textures');
        clearElement(texturesList);
        for (textureName in randar.resources.textures) {
            var element = document.createElement('li');
            element.innerHTML = textureName;
            texturesList.appendChild(element);
        }

        var modelsList = getElement('#objects .models');
        clearElement(modelsList);
        for (modelName in randar.resources.models) {
            var element = document.createElement('li');
            element.innerHTML = modelName;
            modelsList.appendChild(element);
        }
    },

    /**
     * Project resources.
     */
    resources: {
        models: { },
        textures: { },
        shaders: { }
    }
};


/**
 * Generic helpers.
 */
function getElement(query) {
    return document.querySelector(query);
}

function getElements(query) {
    return document.querySelectorAll(query);
}

function hideElement(element) {
    element.classList.add('hidden');
}

function showElement(element) {
    element.classList.remove('hidden');
}

function isString(value) {
    return typeof value === 'string' || value instanceof String;
}

function clearElement(element) {
    element.innerHTML = '';
}


/**
 * Program-specific helpers.
 */
function showMain() {
    var objectLists = getElements('#objects ul');
    for (var i = 0; i < objectLists.length; i++) {
        var element = objectLists[i];

        if (element.className != 'back') {
            hideElement(element);
        }
    }

    hideElement(getElement('#objects'));
    hideElement(getElement('#settings'));
    showElement(getElement('#main'));
}

function showSettings() {
    hideElement(getElement('#main'));
    showElement(getElement('#settings'));
}

function showObjects(category) {
    hideElement(getElement('#main'));
    showElement(getElement('#objects'));

    showElement(getElement('#objects ul.' + category));
}


/**
 * Initialize the interface.
 */
randar.ready = function() {
    showMain();

    getElement('#import-resource').addEventListener('click', function() {
        var results = window.importResource();
        randar.updateResources();

        var message = results ? results.message : 'No file selected.';
        getElement('#import-message').innerHTML = message;
    });

    var backButtons = getElements('nav ul.back');
    for (var i = 0; i < backButtons.length; i++) {
        backButtons[i].addEventListener('click', function() {
            showMain();
        });
    }

    var resourceItems = getElements('#main li');
    for (var i = 0; i < resourceItems.length; i++) {
        var item = resourceItems[i];
        var category = item.className;

        if (category == 'randar') {
            item.addEventListener('click', function() {
                showSettings();
            });
        }

        else {
            item.addEventListener('click', function(category) {
                showObjects(category);
            }.bind(this, category));
        }
    }

    randar.updateResources();
}
