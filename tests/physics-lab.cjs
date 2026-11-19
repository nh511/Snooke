'use strict';
const assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path');
const root=process.argv[2],runs=process.argv[3];assert(root&&runs);
const index=JSON.parse(fs.readFileSync(path.join(root,'manifest.json'),'utf8'));
assert.equal(index.schema,'snooker-physics-lab-index-v1');
assert.deepEqual(index.scenarios,['single-ball','ball-collision','cushion','pocket']);
for(const name of index.scenarios){
  const dir=path.join(root,name),manifest=JSON.parse(fs.readFileSync(path.join(dir,'scenario.json'),'utf8'));
  assert.equal(manifest.schema,'snooker-physics-lab-v1');assert.equal(manifest.name,name);
  assert(fs.existsSync(path.join(dir,'initial.snapshot')));assert(fs.existsSync(path.join(dir,'shot.input')));
  assert(manifest.ballCount>=1);assert(manifest.expectedEvents.length>0);
  const events=fs.readFileSync(path.join(runs,name,'events.txt'),'utf8').trim().split(/\r?\n/).map(line=>Number(line.split(' ')[3]));
  assert.equal(events[0],0);assert.equal(events.at(-1),9);
  if(name==='single-ball'){assert(events.includes(8));assert(!events.includes(1)&&!events.includes(2)&&!events.includes(5));}
  if(name==='ball-collision')assert(events.includes(1));
  if(name==='cushion')assert(events.includes(2));
  if(name==='pocket'){assert(events.includes(5));assert(events.includes(4));assert(events.includes(6));}
}
console.log('PASS independent physics lab manifests, inputs and real service event sequences');
