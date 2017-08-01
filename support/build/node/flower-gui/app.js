var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var multer  = require('multer')
var upload = multer({ dest: 'uploads/' })
var fs = require('fs');
var path = require('path');

var index = require('./routes/index');
var users = require('./routes/users');

var app = express();

var runner = require('./runner');

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
//app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', index);
app.use('/users', users);

app.use(upload.any());

var params = {};
var done = false;
var flower_root = path.join(__dirname, "public", "flower");
var paralogs_dir = path.join(flower_root, "paralogs");
var report_dir = path.join(flower_root, "report");
if (! fs.existsSync(paralogs_dir)) {
  fs.mkdirSync(paralogs_dir);
}
if (! fs.existsSync(report_dir)) {
  fs.mkdirSync(report_dir);
}

app.post('/upload', function (req, res, next) {
  // req.files is array of `photos` files
  // req.body will contain the text fields, if there were any
  var fasta_files = req.files.filter((f) => f.fieldname == 'fasta_files').map((f) => {
    var new_path = path.join(__dirname, 'uploads', f.originalname);
    fs.renameSync(f.path, new_path);
    return new_path;
  });

  var outgroup_file = req.files.filter((f) => f.fieldname == 'outgroup_file').map((f) => {
    var new_path = path.join(__dirname, 'uploads', f.originalname);
    fs.renameSync(f.path, new_path);
    return new_path;
  });
  
  outgroup_file = (outgroup_file && outgroup_file.length) ? outgroup_file[0] : null;
  if (outgroup_file) {
    fasta_files.splice(fasta_files.indexOf(outgroup_file), 1);
  }

  var graph_types = req.body.graph_types;
  graph_types = Array.isArray(graph_types) ? graph_types : [graph_types];

  var output_html = req.body.output_format == 'html';
  params = {
    fasta_files,
    outgroup_file,
    output_html,
    seq_overlap: req.body.seq_overlap,
    seg_overlap: req.body.seg_overlap,
    group_overlap: req.body.group_overlap,
    outgroup_cutoff: req.body.outgroup_cutoff,
    blast_cutoff: req.body.blast_cutoff,
    min_clique_size: req.body.min_clique_size,
    edge_threshold: req.body.edge_threshold,
    graph_types,
    scoring_matrix: req.body.scoring_matrix
  };

  runner.run(params, () => {
    done = true;
  });

  res.redirect("/status");
});

app.get('/status', function(req, res, next) {
  var files = fs.readdirSync(paralogs_dir);
  var sh_files = files.filter((f) => f.match(/\.sh$/));
  var blastout_files = files.filter((f) => f.match(/\.blastout$/));
  var num_combinations = params.fasta_files.length * params.fasta_files.length;
  if (params.outgroup_file) {
    num_combinations = (params.fasta_files.length + 1) * params.fasta_files.length;
  }

  if (done && params.output_html) {
    return res.redirect("/flower/report/index.html");
  } else {
    return res.render('status', { progress: `${Math.floor(100 * blastout_files.length / num_combinations)}`, params, files, sh_files, blastout_files, done, num_combinations});
  }
});

app.get('/download', function(req, res, next) {
  return res.redirect("/flower/report/cliques.report");
});

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
