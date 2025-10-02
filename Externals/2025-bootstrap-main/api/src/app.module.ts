import { Module } from '@nestjs/common';
import { UsersController } from './users.controller';
import { UsersService } from './users.service';
import { GenAIController } from './genai.controller';
import { GenAIService } from './genai.service';
import { ConfigModule } from '@nestjs/config';

@Module({
  imports: [ConfigModule.forRoot()],
  controllers: [UsersController, GenAIController],
  providers: [UsersService, GenAIService],
})
export class AppModule {}
