function saveFeedback(){
    const scriptURL = 'https://script.google.com/macros/s/AKfycbxmUFSr53fiaeyyMQ31Nuirb21hgkv2ApbntHMkuS6quuQ_mEOVdxrI-DNVkWhFsUav0g/exec'
    let data = new FormData();
    const utcStr = new Date().toUTCString();
    console.log(utcStr);
    let msg = document.getElementById('message').value
    data.append('message', msg)            
    data.append('url', window.location.href)
    data.append('timestamp', new Date())

    console.log(data)
    fetch(scriptURL, { method: 'POST', body: data})
    .then(response => alert("Feedback saved successfully"))
    .catch(error => alert("Feedback not saved, try again"))
}

function yes(){
    alert("Glad the documentation was useful!")
}

function no(){
    alert("Thank you for the feedback, feel free to raise an issue on our Github page, we will resolve it soon :)")
}

function feedback(){
    let msg = document.getElementById('feedback').value
    alert(msg) 
}