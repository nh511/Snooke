const assert=require('node:assert/strict');
const fs=require('node:fs');
const {parse,header}=require('../tools/trajectory-data.js');
const row='0,0,1,0,0,0.02625,0.3,0.05,0,0,-2,0.1,0,3.569,1.778,0.02625,engineering-v1';
assert.equal(parse(header+'\n'+row).length,1);
for(const text of ['',header+'\n'+row+'\n'+row,header+'\n'+row.replace('0.3','NaN'),header+'\n'+row.replace('engineering-v1','<script>'),header+'\n'+row+',extra'])assert.throws(()=>parse(text));
if(process.argv[2]){
  const rows=parse(fs.readFileSync(process.argv[2],'utf8'));
  assert(rows.length>2);assert.equal(rows.at(-1).sleeping,1);
  assert.equal(Math.hypot(rows.at(-1).vx,rows.at(-1).vy,rows.at(-1).vz),0);
  assert(rows.at(-1).x>rows[0].x);
  console.log('PASS real CSV rows='+rows.length+' settled='+rows.at(-1).time+'s');
}
console.log('PASS CSV parser rejects invalid/nonordered input');
