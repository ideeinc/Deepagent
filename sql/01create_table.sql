use deepagentdb;

DROP TABLE IF EXISTS solver_proto;
CREATE TABLE solver_proto (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    net VARCHAR(200),
    test_iter VARCHAR(20),
    test_interval VARCHAR(20),
    base_lr VARCHAR(20),
    momentum VARCHAR(20),
    weight_decay  VARCHAR(20),
    lr_policy  VARCHAR(20),
    gamma VARCHAR(20),
    power VARCHAR(20),
    display VARCHAR(20),
    max_iter VARCHAR(20),
    snapshot VARCHAR(20),
    snapshot_prefix VARCHAR(20),
    solver_mode VARCHAR(20),
    updated_at TIMESTAMP DEFAULT 0, 
    lock_revision INTEGER
);

DROP TABLE IF EXISTS caffe_context;
CREATE TABLE caffe_context (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    solver_proto_id INTEGER,
    neural_net_id INTEGER,
    caffe_pid bigint,
    topic VARCHAR(255),
    log_path VARCHAR(255),
    updated_at TIMESTAMP DEFAULT 0,
    lock_revision INTEGER
);

DROP TABLE IF EXISTS workspace;
CREATE TABLE workspace (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    solver_proto_id INTEGER,
    neural_net_id INTEGER,
    log_path VARCHAR(255),
    updated_at TIMESTAMP DEFAULT 0,
    lock_revision INTEGER
);

DROP TABLE IF EXISTS neural_network;
CREATE TABLE neural_network (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(255),
    type VARCHAR(20),  -- deploy, train or original
    -- layers MEDIUMTEXT,
    abs_file_path VARCHAR(4095),
    updated_at TIMESTAMP DEFAULT 0,
    lock_revision INTEGER
);

DROP TABLE IF EXISTS caffe_trained_model;
CREATE TABLE caffe_trained_model (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    model_path VARCHAR(4095),
    neural_network_name VARCHAR(255),
    epoch INTEGER,
    dataset_id INTEGER,  -- get mean data
    img_width INTEGER,
    img_height INTEGER,
    updated_at TIMESTAMP DEFAULT 0,
    lock_revision INTEGER
);

DROP TABLE IF EXISTS dataset;
CREATE TABLE dataset (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    db_path VARCHAR(4095),
    db_type VARCHAR(20), -- LevelDB or LMDB
    mean_path VARCHAR(4095),
    updated_at TIMESTAMP DEFAULT 0,
    lock_revision INTEGER
);

DROP TABLE IF EXISTS class_label;
CREATE TABLE class_label (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    dataset_id INTEGER,
    idx INTEGER,
    name VARCHAR(255),
    meta_info VARCHAR(255),
    updated_at TIMESTAMP DEFAULT 0, 
    lock_revision INTEGER
);