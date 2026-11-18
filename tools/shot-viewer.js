'use strict';
const el=id=>document.getElementById(id),canvas=el('scene'),ctx=canvas.getContext('2d');
const kinds=['Cue strike','Ball collision','Cushion','Jaw','Pocket contact','Mouth entry','Potted','Recovered','Ball settled','Shot settled'];
let data=null,index=0,playing=false,last=0,playTime=0;
function pause(){playing=false;el('play').textContent='\u25b6';}
function draw(){
 const box=canvas.getBoundingClientRect(),ratio=devicePixelRatio||1;canvas.width=Math.round(box.width*ratio);canvas.height=Math.round(box.height*ratio);ctx.setTransform(ratio,0,0,ratio,0,0);ctx.fillStyle='#154c36';ctx.fillRect(0,0,box.width,box.height);if(!data)return;
 const geometry=data.result.geometry,frame=data.frames[index];let xmin=Infinity,xmax=-Infinity,ymin=Infinity,ymax=-Infinity;
 for(const s of geometry.segments)for(const p of [s.start,s.end]){xmin=Math.min(xmin,p[0]);xmax=Math.max(xmax,p[0]);ymin=Math.min(ymin,p[1]);ymax=Math.max(ymax,p[1]);}
 const scale=Math.min((box.width-38)/(xmax-xmin),(box.height-38)/(ymax-ymin)),ox=box.width/2-(xmin+xmax)*scale/2,oy=box.height/2+(ymin+ymax)*scale/2;
 const point=p=>[ox+p[0]*scale,oy-p[1]*scale];
 for(const s of geometry.segments){ctx.strokeStyle=s.kind===4?'#e4a057':s.kind===0?'#a9bdad':'#dbe4de';ctx.lineWidth=s.kind===0?1:2;ctx.beginPath();ctx.moveTo(...point(s.start));ctx.lineTo(...point(s.end));ctx.stroke();}
 for(const p of geometry.pockets){ctx.fillStyle='#111b17';ctx.beginPath();ctx.arc(...point(p.center),p.opening*scale/2,0,Math.PI*2);ctx.fill();}
 const palette=['#f8fbfb','#ed5966','#edcd52','#71c5d0','#bf97dd','#e89c61'];
 if(el('paths').checked)for(let b=0;b<frame.balls.length;b++){ctx.strokeStyle=palette[b%palette.length];ctx.globalAlpha=.65;ctx.lineWidth=1;ctx.beginPath();for(let n=0;n<=index;n++){const p=point(data.frames[n].balls[b].position);n?ctx.lineTo(...p):ctx.moveTo(...p);}ctx.stroke();}ctx.globalAlpha=1;
 if(el('contacts').checked)for(const e of data.result.events)if(e.time<=frame.time&&e.kind>0&&e.kind<8){const p=point(e.point);ctx.fillStyle=e.kind===6?'#f2da58':'#73dfde';ctx.fillRect(p[0]-2,p[1]-2,4,4);}
 for(let b=0;b<frame.balls.length;b++){const ball=frame.balls[b];if(ball.pocketState===3)continue;const p=point(ball.position),r=Math.max(3,data.result.final.balls[b].radius*scale);ctx.fillStyle=palette[b%palette.length];ctx.beginPath();ctx.arc(...p,r,0,Math.PI*2);ctx.fill();ctx.strokeStyle='#153829';ctx.stroke();if(el('vectors').checked){ctx.strokeStyle='#fff';ctx.beginPath();ctx.moveTo(...p);ctx.lineTo(p[0]+ball.velocity[0]*scale*.3,p[1]-ball.velocity[1]*scale*.3);ctx.stroke();}}
 el('time').value=index;el('step').textContent=frame.step;el('balls').textContent=frame.balls.filter(b=>b.pocketState!==3).length;el('eventCount').textContent=data.result.events.filter(e=>e.time<=frame.time).length;el('counter').textContent=frame.time.toFixed(3)+' / '+data.frames.at(-1).time.toFixed(3)+' s';el('back').disabled=index===0;el('next').disabled=index===data.frames.length-1;
}
window.loadShotData=function(json,csv){const candidate=SnookerShotData.parse(json,csv);pause();data=candidate;index=0;el('error').textContent='';el('status').textContent=data.result.parameterVersion;el('time').max=data.frames.length-1;el('time').disabled=false;el('play').disabled=false;el('events').replaceChildren();data.result.events.forEach((e,n)=>{const option=document.createElement('option');option.value=n;option.textContent=e.time.toFixed(3)+' | '+(kinds[e.kind]||e.kind)+' | '+e.ball;el('events').append(option);});draw();};
window.shotViewerState=()=>data?{index,frames:data.frames.length,balls:data.result.final.balls.length,events:data.result.events.length}:null;
el('files').onchange=async e=>{try{const files=[...e.target.files],csv=files.find(f=>f.name.endsWith('.csv')),json=files.find(f=>f.name==='result.json')||files.find(f=>f.name.endsWith('.json'));if(!csv||!json)throw Error('Select result.json and trajectory.csv');if(csv.size>64*1024*1024||json.size>16*1024*1024)throw Error('File size limit exceeded');loadShotData(await json.text(),await csv.text());}catch(error){pause();el('error').textContent=error.message;}};
el('events').onchange=e=>{if(!data)return;pause();const event=data.result.events[Number(e.target.value)];index=data.frames.findIndex(f=>f.time+1e-12>=event.time);if(index<0)index=data.frames.length-1;draw();};
el('back').onclick=()=>{pause();index=Math.max(0,index-1);draw();};el('next').onclick=()=>{pause();index=Math.min(data.frames.length-1,index+1);draw();};el('time').oninput=e=>{pause();index=Number(e.target.value);draw();};
el('play').onclick=()=>{if(playing)pause();else{if(index===data.frames.length-1)index=0;playing=true;playTime=data.frames[index].time;last=performance.now();el('play').textContent='\u2161';}};
for(const id of ['paths','contacts','vectors'])el(id).onchange=draw;
function tick(now){if(playing&&data){playTime+=(now-last)/1000;last=now;while(index<data.frames.length-1&&data.frames[index+1].time<=playTime)index++;if(index===data.frames.length-1)pause();draw();}requestAnimationFrame(tick);}requestAnimationFrame(tick);addEventListener('resize',draw);draw();
