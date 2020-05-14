Module['onRuntimeInitialized'] = function(text) {
    wasmLoadedCallback();

    if (!ENVIRONMENT_IS_PTHREAD) {
        PThread.receiveObjectTransfer = function (data) {
            let event = new CustomEvent('worker_message', {detail : data.msg});
            window.dispatchEvent(event);
        };
    }
};

addEventListener('worker_message', function(e) {
    if (e.detail == 'step_done') {
        stepCompletedCallback();
    }
});
