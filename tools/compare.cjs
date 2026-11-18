'use strict';
const fs=require('node:fs'),path=require('node:path');
function trajectory(text) {
  const lines=text.trim().split(/\r?\n/),header=lines.shift().split(',');
  if(header.length!==17 || header[0]!=='step' || header[16]!=='event_count')throw Error('unsupported trajectory schema');
  const rows=lines.map(line=>{const v=line.split(',').map(Number);if(v.length!==17 || !v.every(Number.isFinite))throw Error('malformed trajectory row');return v;});
  if(!rows.length)throw Error('empty trajectory');
  for(let i=0;i<rows.length;i++)if(i && (rows[i][0]<rows[i-1][0] || (rows[i][0]===rows[i-1][0] && rows[i][2]<=rows[i-1][2])))throw Error('unordered trajectory');
  return {header,rows};
}
function compare(a,b,tolerance=0) {
  if(!Number.isFinite(tolerance) || tolerance<0)throw Error('invalid comparison tolerance');
  const expected=trajectory(a),actual=trajectory(b);
  if(expected.header.join(',')!==actual.header.join(','))return {pass:false,field:'header'};
  const exact=new Set([0,2,12,13,14,15,16]);
  for(let i=0;i<Math.min(expected.rows.length,actual.rows.length);i++)for(let j=0;j<17;j++) {
    const x=expected.rows[i][j],y=actual.rows[i][j];
    if(Math.abs(x-y)>(exact.has(j)?0:tolerance))return {pass:false,step:expected.rows[i][0],ball:expected.rows[i][2],field:expected.header[j],expected:x,actual:y};
  }
  return expected.rows.length===actual.rows.length?{pass:true,rows:expected.rows.length}:{pass:false,field:'row_count',expected:expected.rows.length,actual:actual.rows.length};
}
if(require.main===module) {
  try {
    if(process.argv.length<4 || process.argv.length>5)throw Error('usage: node tools/compare.cjs expected.csv actual.csv [tolerance]');
    const result=compare(fs.readFileSync(process.argv[2],'utf8'),fs.readFileSync(process.argv[3],'utf8'),Number(process.argv[4]||0));
    console.log((result.pass?'PASS ':'FAIL ')+JSON.stringify(result));process.exitCode=result.pass?0:1;
  }catch(error){console.error('ERROR '+error.message);process.exitCode=2;}
}
module.exports={compare,trajectory};
