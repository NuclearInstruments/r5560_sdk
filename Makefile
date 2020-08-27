.PHONY: clean All

All:
	@echo "----------Building project:[ R5560_SDKLib - Debug ]----------"
	@cd "R5560_SDKLib" && "$(MAKE)" -f  "libr5560.mk"
	@echo "----------Building project:[ R5560_SDKApp - Debug ]----------"
	@cd "R5560_SDKApp" && "$(MAKE)" -f  "r5560sdk_libtest.mk"
clean:
	@echo "----------Building project:[ R5560_SDKLib - Debug ]----------"
	@cd "R5560_SDKLib" && "$(MAKE)" -f  "libr5560.mk" clean
	@echo "----------Building project:[ R5560_SDKApp - Debug ]----------"
	@cd "R5560_SDKApp" && "$(MAKE)" -f  "r5560sdk_libtest.mk" clean
