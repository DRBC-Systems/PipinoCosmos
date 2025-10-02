import { Injectable } from '@nestjs/common';
import { User } from './user.model';
import * as fs from 'fs';

@Injectable()
export class UsersService {
  getList(): User[] | null {
    const filePath = process.cwd() + '/data/users.json';

    try {
      const data = JSON.parse(fs.readFileSync(filePath).toString());
      const list: User[] = [];

      for (const item of data) {
        const user = new User(item.id, item.firstName, item.lastName);
        list.push(user);
      }

      return list;
    } catch ( err ) {
      return null;
    }
  }
}
