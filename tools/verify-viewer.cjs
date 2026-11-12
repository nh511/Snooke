const {spawn}=require('node:child_process');
const fs=require('node:fs');
const path=require('node:path');
const {pathToFileURL}=require('node:url');
const assert=require('node:assert/strict');
async function main(){
  const chrome=process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe';
  const evidence=path.resolve('build-evidence','viewer-'+Date.now());
  fs.mkdirSync(evidence,{recursive:true});
  const child=spawn(chrome,['--headless=new','--disable-gpu','--no-first-run','--no-default-browser-check',
    '--remote-debugging-port=0','--remote-debugging-address=127.0.0.1','--user-data-dir='+path.join(evidence,'profile'),'about:blank'],
    {windowsHide:true,stdio:['ignore','ignore','pipe']});
  let socket;
  try{
    const browserURL=await new Promise((resolve,reject)=>{
      let text='';const timeout=setTimeout(()=>reject(new Error('Chrome startup timeout')),20000);
      child.on('error',reject);child.on('exit',code=>reject(new Error('Chrome exited '+code)));
      child.stderr.on('data',bytes=>{text+=bytes.toString();const marker='DevTools listening on ';const start=text.indexOf(marker);if(start>=0){const address=text.slice(start+marker.length).trim().split(String.fromCharCode(10))[0].trim();if(address.startsWith('ws:')){clearTimeout(timeout);resolve(address);}}});
    });
    const port=new URL(browserURL).port;
    const pages=await (await fetch('http://127.0.0.1:'+port+'/json/list')).json();
    const page=pages.find(p=>p.type==='page');assert(page);
    socket=new WebSocket(page.webSocketDebuggerUrl);
    await new Promise((resolve,reject)=>{socket.onopen=resolve;socket.onerror=reject;});
    let id=0;const pending=new Map(),errors=[];
    socket.onmessage=event=>{
      const value=JSON.parse(event.data);
      if(value.id){const p=pending.get(value.id);if(p){pending.delete(value.id);clearTimeout(p.timeout);value.error?p.reject(new Error(JSON.stringify(value.error))):p.resolve(value.result);}}
      else if(value.method==='Runtime.exceptionThrown') errors.push(value.params.exceptionDetails.text);
    };
    const command=(method,params={})=>new Promise((resolve,reject)=>{
      const request=++id;const timeout=setTimeout(()=>{pending.delete(request);reject(new Error('CDP timeout '+method));},15000);
      pending.set(request,{resolve,reject,timeout});socket.send(JSON.stringify({id:request,method,params}));
    });
    const evaluate=async expression=>{
      const value=await command('Runtime.evaluate',{expression,returnByValue:true,awaitPromise:true});
      if(value.exceptionDetails)throw new Error(JSON.stringify(value.exceptionDetails));
      return value.result.value;
    };
    await command('Page.enable');await command('Runtime.enable');
    await command('Emulation.setDeviceMetricsOverride',{width:1280,height:800,deviceScaleFactor:1,mobile:false});
    await command('Page.navigate',{url:pathToFileURL(path.resolve('tools/trajectory.html')).href});
    for(let attempt=0;attempt<100;attempt++){
      if(await evaluate('typeof window.loadTrajectory === "function"'))break;
      if(attempt===99)throw new Error('Viewer did not load');
      await new Promise(r=>setTimeout(r,50));
    }
    const csv=fs.readFileSync('build/trajectory.csv','utf8');
    await evaluate('loadTrajectory('+JSON.stringify(csv)+',"Actual single-ball run")');
    await evaluate("document.getElementById('time').value=document.getElementById('time').max;document.getElementById('time').dispatchEvent(new Event('input'))");
    assert.equal(await evaluate("document.getElementById('state').textContent"),'Sleeping');
    const end=await evaluate("Number(document.getElementById('step').textContent)");
    await evaluate("document.getElementById('back').click()");
    assert.equal(await evaluate("Number(document.getElementById('step').textContent)"),end-1);
    await evaluate("document.getElementById('next').click()");
    const colors=await evaluate("(()=>{const c=document.getElementById('scene'),p=c.getContext('2d').getImageData(0,0,c.width,c.height).data,s=new Set();for(let i=0;i<p.length;i+=16)s.add(p[i]+','+p[i+1]+','+p[i+2]);return s.size;})()");
    assert(colors>4);assert.equal(await evaluate('document.documentElement.scrollWidth>innerWidth'),false);
    const desktop=await command('Page.captureScreenshot',{format:'png'});
    fs.writeFileSync(path.join(evidence,'desktop.png'),Buffer.from(desktop.data,'base64'));
    await evaluate("document.getElementById('time').value=0;document.getElementById('time').dispatchEvent(new Event('input'));document.getElementById('play').click()");
    await new Promise(r=>setTimeout(r,250));
    assert(await evaluate("Number(document.getElementById('step').textContent)>0"));
    await evaluate("document.getElementById('play').click()");
    await command('Emulation.setDeviceMetricsOverride',{width:390,height:844,deviceScaleFactor:1,mobile:true});
    await evaluate("window.dispatchEvent(new Event('resize'))");
    assert.equal(await evaluate('document.documentElement.scrollWidth>innerWidth'),false);
    const mobile=await command('Page.captureScreenshot',{format:'png'});
    fs.writeFileSync(path.join(evidence,'mobile.png'),Buffer.from(mobile.data,'base64'));
    assert.deepEqual(errors,[]);
    fs.writeFileSync(path.join(evidence,'result.json'),JSON.stringify({end,colors,errors,desktop:'1280x800',mobile:'390x844'},null,2));
    console.log('PASS real Chrome CSV load, canvas pixels, previous/next/play, desktop/mobile; '+evidence);
  }finally{
    if(socket)socket.close();
    child.kill();
    if(child.exitCode===null)await new Promise(resolve=>{child.once('exit',resolve);setTimeout(resolve,5000);});
  }
}
main().catch(error=>{console.error(error);process.exitCode=1;});
