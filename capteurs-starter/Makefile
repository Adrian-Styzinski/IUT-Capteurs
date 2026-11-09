CC ?= cc
CPPFLAGS ?= -Iinclude -Iprovided/cjson
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g
LDFLAGS ?=
LDLIBS ?= -lm -lcurl

VALGRIND ?= valgrind
VALGRIND_FLAGS ?= \
	--leak-check=full \
	--show-leak-kinds=all \
	--errors-for-leak-kinds=definite,indirect,possible \
	--track-origins=yes \
	--error-exitcode=1

ARGS ?=
SCENARIO_ARGS ?=

BUILD_DIR := build
PROGRAM := $(BUILD_DIR)/capteurs
PROJECT_PDF := $(BUILD_DIR)/projet-capteurs.pdf
TEST_STATISTICS := $(BUILD_DIR)/test_statistics
TEST_SERIES := $(BUILD_DIR)/test_series
TEST_SOURCE := $(BUILD_DIR)/test_source
TEST_REPORT := $(BUILD_DIR)/test_report
TEST_CSV := $(BUILD_DIR)/test_csv
LIB_OBJS := \
	$(BUILD_DIR)/series.o \
	$(BUILD_DIR)/statistics.o \
	$(BUILD_DIR)/sensor_source.o \
	$(BUILD_DIR)/report.o \
	$(BUILD_DIR)/cJSON.o

.PHONY: all run test test-statistics test-series test-source test-report test-csv test-cli memcheck memcheck-tests memcheck-series memcheck-cli memcheck-run memcheck-scenario check-tools cppcheck pdf clean

all: $(PROGRAM)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/main.o: src/main.c include/projet/report.h include/projet/sensor_source.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/series.o: src/series.c include/projet/series.h include/projet/measure.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/statistics.o: src/statistics.c include/projet/statistics.h include/projet/series.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/sensor_source.o: provided/sensor_source.c include/projet/sensor_source.h include/projet/series.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cJSON.o: provided/cjson/cJSON.c provided/cjson/cJSON.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/report.o: src/report.c include/projet/report.h include/projet/statistics.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_statistics.o: tests/test_statistics.c include/projet/statistics.h include/projet/series.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_series.o: tests/test_series.c include/projet/series.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_source.o: tests/test_source.c include/projet/sensor_source.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_report.o: tests/test_report.c include/projet/report.h include/projet/statistics.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_csv.o: tests/test_csv.c include/projet/report.h include/projet/statistics.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(PROGRAM): $(BUILD_DIR)/main.o $(LIB_OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST_STATISTICS): $(BUILD_DIR)/test_statistics.o $(BUILD_DIR)/statistics.o
	$(CC) $(LDFLAGS) $^ -lm -o $@

$(TEST_SERIES): $(BUILD_DIR)/test_series.o $(BUILD_DIR)/series.o

	$(CC) $(LDFLAGS) $^ -o $@

$(TEST_SOURCE): $(BUILD_DIR)/test_source.o $(BUILD_DIR)/series.o $(BUILD_DIR)/sensor_source.o $(BUILD_DIR)/cJSON.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST_REPORT): $(BUILD_DIR)/test_report.o $(BUILD_DIR)/report.o $(BUILD_DIR)/statistics.o
	$(CC) $(LDFLAGS) $^ -lm -o $@

$(TEST_CSV): $(BUILD_DIR)/test_csv.o $(BUILD_DIR)/report.o $(BUILD_DIR)/statistics.o
	$(CC) $(LDFLAGS) $^ -lm -o $@

run: $(PROGRAM)
	./$(PROGRAM)

test: test-statistics test-series test-source test-report test-csv test-cli

test-statistics: $(TEST_STATISTICS)
	./$(TEST_STATISTICS)

test-series: $(TEST_SERIES)
	./$(TEST_SERIES)

test-source: $(TEST_SOURCE)
	./$(TEST_SOURCE)

test-report: $(TEST_REPORT)
	./$(TEST_REPORT)
	@echo "test-report: succes."

test-csv: test-report $(TEST_CSV)
	./$(TEST_CSV)
	@echo "test-csv: succes."

test-cli: test-csv $(PROGRAM)
	sh tests/test_cli.sh ./$(PROGRAM)

memcheck: memcheck-tests memcheck-cli

memcheck-series: $(TEST_SERIES)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(TEST_SERIES)

memcheck-tests: $(TEST_STATISTICS) $(TEST_SERIES) $(TEST_SOURCE) $(TEST_REPORT) $(TEST_CSV)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(TEST_STATISTICS)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(TEST_SERIES)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(TEST_SOURCE)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(TEST_REPORT)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(TEST_CSV)

memcheck-cli: $(PROGRAM)
	@csv_file=$$(mktemp); trap 'rm -f "$$csv_file"' EXIT HUP INT TERM; \
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(PROGRAM) --label "Memcheck CLI" \
		--csv "$$csv_file" --file tests/data/sensors-history.json; \
	test -s "$$csv_file"

memcheck-run: $(PROGRAM)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(PROGRAM) $(ARGS)

memcheck-scenario: $(PROGRAM)
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(PROGRAM) $(SCENARIO_ARGS)

check-tools:
	@command -v $(CC) >/dev/null || { echo "Compilateur C introuvable: $(CC)"; exit 1; }
	@command -v $(MAKE) >/dev/null || { echo "Make introuvable"; exit 1; }
	@command -v cppcheck >/dev/null || { echo "cppcheck introuvable"; exit 1; }
	@command -v $(VALGRIND) >/dev/null || { echo "Valgrind introuvable"; exit 1; }
	@echo "Environnement de développement prêt."

cppcheck:
	cppcheck --enable=warning,style,performance,portability --inline-suppr --std=c11 --error-exitcode=1 --suppress=missingIncludeSystem -Iinclude src

pdf: SUJET.md | $(BUILD_DIR)
	pandoc --standalone --metadata title="Projet capteurs SAE-C" SUJET.md -o $(PROJECT_PDF)

clean:
	rm -rf $(BUILD_DIR)
