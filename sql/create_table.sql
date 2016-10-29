create table solver_proto (id INTEGER PRIMARY KEY AUTO_INCREMENT, net varchar(200), test_iter varchar(20), test_interval varchar(20), base_lr varchar(20), momentum varchar(20), weight_decay  varchar(20), lr_policy  varchar(20), gamma varchar(20),  power varchar(20), display varchar(20), max_iter varchar(20), snapshot varchar(20), snapshot_prefix varchar(20), solver_mode varchar(20), created_at TIMESTAMP DEFAULT 0, updated_at TIMESTAMP DEFAULT 0, lock_revision INTEGER);

create table caffe_context (id INTEGER PRIMARY KEY AUTO_INCREMENT, solver_proto_id INTEGER, neural_net_id INTEGER, caffe_pid bigint, topic varchar(50), log_path varchar(200), created_at TIMESTAMP DEFAULT 0, lock_revision INTEGER);

create table workspace (id INTEGER PRIMARY KEY AUTO_INCREMENT, solver_proto_id INTEGER, neural_net_id INTEGER, log_path varchar(200), created_at TIMESTAMP DEFAULT 0, updated_at TIMESTAMP DEFAULT 0, lock_revision INTEGER);

create table neural_network (id INTEGER PRIMARY KEY AUTO_INCREMENT, layers MEDIUMTEXT, created_at TIMESTAMP DEFAULT 0, updated_at TIMESTAMP DEFAULT 0, lock_revision INTEGER);

