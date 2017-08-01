'use strict'

var exec = require('child_process').execFile;
var path = require('path');

/*
{ 
  fasta_files: [ 'uploads/a.fasta', 'uploads/b.fasta', 'uploads/c.fasta' ],
  outgroup_file: 'uploads/c.fasta',
  output_html: true,
  seq_overlap: req.body.seq_overlap,
  seg_overlap: req.body.seg_overlap,
  group_overlap: req.body.group_overlap,
  outgroup_cutoff: req.body.outgroup_cutoff,
  blast_cutoff: req.body.blast_cutoff,
  min_clique_size: req.body.min_clique_size,
  edge_threshold: req.body.edge_threshold,
  graph_types: ["D", "I", "C"]
  scoring_matrix: req.body.scoring_matrix
}
*/
var flower_root = path.join(__dirname, "public", "flower");
var paralogs_dir = path.join(flower_root, "paralogs");
var report_dir = path.join(flower_root, "report");

function run(params, callback) {
  var args = ['-s'];
  args = args.concat(params.fasta_files);
  args = args.concat(['-p', paralogs_dir, '-R', report_dir]);
  if (params.outgroup_file) {
    args.push('-o');
    args.push(params.outgroup_file);
  }
  if (params.output_html) {
    args.push('-H');
  }
  args = args.concat(['-q', params.seq_overlap]);
  args = args.concat(['-g', params.seg_overlap]);
  args = args.concat(['-r', params.group_overlap]);
  args = args.concat(['-c', params.outgroup_cutoff]);
  args = args.concat(['-b', params.blast_cutoff]);
  args = args.concat(['-M', params.min_clique_size]);
  args = args.concat(['-E', params.edge_threshold]);
  args = args.concat(['-T', params.graph_types.join("")]);
  args = args.concat(['-m', params.scoring_matrix]);


  exec('flower', args, {}, (err, stdout, stderr) => {
    if (err) {
      console.error("ERROR: " + err);
      throw err;
    }
    callback();
  });

}

module.exports = { run };
