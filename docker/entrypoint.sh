#!/bin/sh
set -x

echo "config start ..."

# config bucketParams

if [ -n "${MAX_REQUESTS}" ]; then     
    sed -i "s#\${MAX_REQUESTS}#${MAX_REQUESTS}#g" config.json
else
    sed -i "s#\${MAX_REQUESTS}#20#g" config.json
fi

# config caffeParams

if [ -n "${GPU_DEVICES}" ]; then     
    sed -i "s#\${GPU_DEVICES}#${GPU_DEVICES}#g" config.json
else
    sed -i "s#\${GPU_DEVICES}#1#g" config.json
fi

if [ -n "${CLASSIFY_THREADS}" ]; then     
    sed -i "s#\${CLASSIFY_THREADS}#${CLASSIFY_THREADS}#g" config.json
else
    sed -i "s#\${CLASSIFY_THREADS}#1#g" config.json
fi

if [ -n "${DETECT_THREADS}" ]; then     
    sed -i "s#\${DETECT_THREADS}#${DETECT_THREADS}#g" config.json
else
    sed -i "s#\${DETECT_THREADS}#2#g" config.json
fi

if [ -n "${FEATURE_THREADS}" ]; then     
    sed -i "s#\${FEATURE_THREADS}#${FEATURE_THREADS}#g" config.json
else
    sed -i "s#\${FEATURE_THREADS}#1#g" config.json
fi
if [ -n "${SEARCH_THREADS}" ]; then     
    sed -i "s#\${SEARCH_THREADS}#${SEARCH_THREADS}#g" config.json
else
    sed -i "s#\${SEARCH_THREADS}#1#g" config.json
fi

if [ -n "${BATCH_SIZE}" ]; then     
    sed -i "s#\${BATCH_SIZE}#${BATCH_SIZE}#g" config.json
else
    sed -i "s#\${BATCH_SIZE}#10#g" config.json
fi

if [ -n "${THREADS_LIVE}" ]; then     
    sed -i "s#\${THREADS_LIVE}#${THREADS_LIVE}#g" config.json
else
    sed -i "s#\${THREADS_LIVE}#1#g" config.json
fi

if [ -n "${THREADS_PERSON}" ]; then     
    sed -i "s#\${THREADS_PERSON}#${THREADS_PERSON}#g" config.json
else
    sed -i "s#\${THREADS_PERSON}#1#g" config.json
fi

# config faceDataParams

if [ -n "${SERVER_URL}" ]; then     
    sed -i "s#\${SERVER_URL}#${SERVER_URL}#g" config.json
else
    sed -i "s#\${SERVER_URL}#127.0.0.1#g" config.json
fi

if [ -n "${SERVER_PORT}" ]; then     
    sed -i "s#\${SERVER_PORT}#${SERVER_PORT}#g" config.json
else
    sed -i "s#\${SERVER_PORT}#8989#g" config.json
fi

if [ -n "${SWITCH}" ]; then     
    sed -i "s#\${SWITCH}#${SWITCH}#g" config.json
else
    sed -i "s#\${SWITCH}#0#g" config.json
fi

if [ -n "${UPLOAD_TIMEPOINT}" ]; then     
    sed -i "s#\${UPLOAD_TIMEPOINT}#${UPLOAD_TIMEPOINT}#g" config.json
else
    sed -i "s#\${UPLOAD_TIMEPOINT}#3:0:0#g" config.json
fi

# config httpParams

sed -i "s#\${API_KEY}#${API_KEY}#g" config.json

if [ -n "${HTTP_THREADS}" ]; then     
    sed -i "s#\${HTTP_THREADS}#${HTTP_THREADS}#g" config.json
else
    sed -i "s#\${HTTP_THREADS}#2#g" config.json
fi

if [ -n "${LISTEN_IP}" ]; then     
    sed -i "s#\${LISTEN_IP}#${LISTEN_IP}#g" config.json
else
    sed -i "s#\${LISTEN_IP}#0.0.0.0#g" config.json
fi

if [ -n "${PORT}" ]; then     
    sed -i "s#\${PORT}#${PORT}#g" config.json
else
    sed -i "s#\${PORT}#33388#g" config.json
fi

#config logConfigParams
if [ -n "${DEBUG_ENABLED}" ]; then     
    sed -i "s#\${DEBUG_ENABLED}#${DEBUG_ENABLED}#g" config.json
else
    sed -i "s#\${DEBUG_ENABLED}#false#g" config.json
fi

if [ -n "${ERROR_ENABLED}" ]; then     
    sed -i "s#\${ERROR_ENABLED}#${ERROR_ENABLED}#g" config.json
else
    sed -i "s#\${ERROR_ENABLED}#false#g" config.json
fi

if [ -n "${FATAL_ENABLED}" ]; then     
    sed -i "s#\${FATAL_ENABLED}#${FATAL_ENABLED}#g" config.json
else
    sed -i "s#\${FATAL_ENABLED}#false#g" config.json
fi

if [ -n "${INFO_ENABLED}" ]; then     
    sed -i "s#\${INFO_ENABLED}#${INFO_ENABLED}#g" config.json
else
    sed -i "s#\${INFO_ENABLED}#true#g" config.json
fi

if [ -n "${TRACE_ENABLED}" ]; then     
    sed -i "s#\${TRACE_ENABLED}#${TRACE_ENABLED}#g" config.json
else
    sed -i "s#\${TRACE_ENABLED}#false#g" config.json
fi

if [ -n "${VERBOSE_ENABLED}" ]; then     
    sed -i "s#\${VERBOSE_ENABLED}#${VERBOSE_ENABLED}#g" config.json
else
    sed -i "s#\${VERBOSE_ENABLED}#false#g" config.json
fi

if [ -n "${WARNING_ENABLED}" ]; then     
    sed -i "s#\${WARNING_ENABLED}#${WARNING_ENABLED}#g" config.json
else
    sed -i "s#\${WARNING_ENABLED}#false#g" config.json
fi

# config searchToolParams
if [ -n "${DISTANCE}" ]; then     
    sed -i "s#\${DISTANCE}#${DISTANCE}#g" config.json
else
    sed -i "s#\${DISTANCE}#250#g" config.json
fi

if [ -n "${REDIS_HOST}" ]; then     
    sed -i "s#\${REDIS_HOST}#${REDIS_HOST}#g" config.json
else
    sed -i "s#\${REDIS_HOST}#127.0.0.1#g" config.json
fi

if [ -n "${REDIS_PORT}" ]; then     
    sed -i "s#\${REDIS_PORT}#${REDIS_PORT}#g" config.json
else
    sed -i "s#\${REDIS_PORT}#6379#g" config.json
fi  

sed -i "s#\${REDIS_PWD}#${REDIS_PWD}#g" config.json

if [ -n "${REDIS_DB_ID}" ]; then     
    sed -i "s#\${REDIS_DB_ID}#${REDIS_DB_ID}#g" config.json
else
    sed -i "s#\${REDIS_DB_ID}#5#g" config.json
fi

if [ -n "${SEARCH_THRESHOLD}" ]; then     
    sed -i "s#\${SEARCH_THRESHOLD}#${SEARCH_THRESHOLD}#g" config.json
else
    sed -i "s#\${SEARCH_THRESHOLD}#0.75#g" config.json
fi

if [ -n "${SIM_THRESHOLD}" ]; then     
    sed -i "s#\${SIM_THRESHOLD}#${SIM_THRESHOLD}#g" config.json
else
    sed -i "s#\${SIM_THRESHOLD}#0.6600000262260437#g" config.json
fi

if [ -n "${TOPK}" ]; then     
    sed -i "s#\${TOPK}#${TOPK}#g" config.json
else
    sed -i "s#\${TOPK}#80#g" config.json
fi

# config zmqParams

if [ -n "${ZMQ_THREADS}${ZMQ_THREADS}" ]; then     
    sed -i "s#\${ZMQ_THREADS}#${ZMQ_THREADS}#g" config.json
else
    sed -i "s#\${ZMQ_THREADS}#2#g" config.json
fi

if [ -n "${ZMQ_ADDR}" ]; then     
    sed -i "s#\${ZMQ_ADDR}#${ZMQ_ADDR}#g" config.json
else
    sed -i "s#\${ZMQ_ADDR}#127.0.0.1#g" config.json
fi

if [ -n "${ZMQ_PORT}" ]; then     
    sed -i "s#\${ZMQ_PORT}#${ZMQ_PORT}#g" config.json
else
    sed -i "s#\${ZMQ_PORT}#5560#g" config.json
fi

if [ -n "${ZMQ_ADDR2}" ]; then     
    sed -i "s#\${ZMQ_ADDR2}#${ZMQ_ADDR2}#g" config.json
else
    sed -i "s#\${ZMQ_ADDR2}#*#g" config.json
fi

if [ -n "${ZMQ_PORT2}" ]; then     
    sed -i "s#\${ZMQ_PORT2}#${ZMQ_PORT2}#g" config.json
else
    sed -i "s#\${ZMQ_PORT2}#5050#g" config.json
fi

echo "config succeed !!"

cat config.json
# start verifier
./verifier