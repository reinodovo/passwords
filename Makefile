manual:
	-mkdir ./manual/pages
	pio run -t exec -e manual -a $(SEED)
	python3 ./manual/generate_manual.py