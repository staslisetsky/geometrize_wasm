Module['onRuntimeInitialized'] = function(text) {
    console.log('Wasm runtime initialized');
};

function appInitialized() {
    console.log('app initialized!');
}

// function geometrizeLoadImage(file) {
//         const file = e.currentTarget.files[0];
//        if(!(file instanceof Blob)) return;
//        const reader = new FileReader();
//        reader.onloadend = evt => {

//            //evt.target.result is an ArrayBuffer. In js,
//            //you can't do anything with an ArrayBuffer
//            //so we have to ???cast??? it to an Uint8Array
//            const uint8_t_arr = new Uint8Array(evt.target.result);

//            //Right now, we have the file as a unit8array in javascript memory.
//            //As far as I understand, wasm can't directly access javascript memory.
//            //Which is why we need to allocate special wasm memory and then
//            //copy the file from javascript memory into wasm memory so our wasm functions
//            //can work on it.

//            //First we need to allocate the wasm memory.
//            //_malloc returns the address of the new wasm memory as int32.
//            //This call is probably similar to
//            //uint8_t * ptr = new uint8_t[sizeof(uint8_t_arr)/sizeof(uint8_t_arr[0])]
//            const uint8_t_ptr = window.Module._malloc(uint8_t_arr.length);

//            //Now that we have a block of memory we can copy the file data into that block
//            //This is probably similar to
//            //std::memcpy(uint8_t_ptr, uint8_t_arr, sizeof(uint8_t_arr)/sizeof(uint8_t_arr[0]))
//            window.Module.HEAPU8.set(uint8_t_arr, uint8_t_ptr);

//            //The only thing that's now left to do is pass
//            //the address of the wasm memory we just allocated
//            //to our function as well as the size of our memory.
//            window.Module.readFile(uint8_t_ptr, uint8_t_arr.length);

//            //At this point we're forced to wait until wasm is done with the memory.
//            //Your site will now freeze if the memory you're working on is big.
//            //Maybe we can somehow let our wasm function run on a seperate thread and pass a callback?

//            //Retreiving our (modified) memory is also straight forward.
//            //First we get some javascript memory and then we copy the
//            //relevant chunk of the wasm memory into our javascript object.
//            const returnArr = new Uint8Array(uint8_t_arr.length);
//            //If returnArr is std::vector<uint8_t>, then is probably similar to
//            //returnArr.assign(ptr, ptr + dataSize)
//            returnArr.set(window.Module.HEAPU8.subarray(uint8_t_ptr, uint8_t_ptr + uint8_t_arr.length));

//            //Lastly, according to the docs, we should call ._free here.
//            //Do we need to call the gc somehow?
//            window.Module._free(uint8_t_ptr);

//        }
//        reader.readAsArrayBuffer(file);
// }